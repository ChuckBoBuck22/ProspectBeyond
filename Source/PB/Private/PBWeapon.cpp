// Fill out your copyright notice in the Description page of Project Settings.


#include "PBWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Engine/SkeletalMesh.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "PB/Public/PB.h"
#include "GameFramework/DamageType.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"


static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeapons(
	TEXT("PB.DebugWeapons"), 
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for Weapons"), 
	ECVF_Cheat);

// Sets default values
APBWeapon::APBWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MuzzleComp = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TraceTargetName = "BeamEndLocation";
	TraceStartName = "BeamStartLocation";
	BaseDamage = 20.f;
	RateOfFire = 600.f;
	bReplicates = true;
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
	BulletSpread = 1.0f;
	Range = 150;

}

void APBWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60/RateOfFire;
}


void APBWeapon::Fire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
		
	}
	AActor* owner = GetOwner();
	if (owner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		owner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		// Bullet Spread
		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

		FVector TraceEnd = EyeLocation + (ShotDirection * Range * 100);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(owner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		// Particle "Target" parameter
		FVector TracerEndPoint = TraceEnd;

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, HITSCAN_COLLISION, QueryParams))
		{
			// Blocking hit! Process damage
			AActor* HitActor = Hit.GetActor();

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, owner->GetInstigatorController(), owner, DamageType);

			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

			TracerEndPoint = Hit.ImpactPoint;

		}

		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TracerEndPoint, FColor::MakeRandomColor(), false, 1.0f, 10.0f);
		}

		PlayFireEffects(TracerEndPoint);

		if (GetLocalRole() == ROLE_Authority)
		{
			HitScanTrace.TraceEnd = TracerEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;
		}

		LastFiredTime = GetWorld()->TimeSeconds;
	}

	
}

void APBWeapon::ServerFire_Implementation()
{
	Fire();
}

bool APBWeapon::ServerFire_Validate()
{
	return true;
}

void APBWeapon::OnRep_HitScanTrace()
{
	PlayFireEffects(HitScanTrace.TraceEnd);
	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceEnd);
}

void APBWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UNiagaraSystem* SelectedEffect = nullptr;
	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
		SelectedEffect = FleshImpactEffect;
		break;
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = VFleshImpactEffect;
		break;
	case SURFACE_ROCK:
		SelectedEffect = RockImpactEffect;
		break;
	case SURFACE_CONCRETE:
		SelectedEffect = ConcreteImpactEffect;
		break;
	case SURFACE_DIRT:
		SelectedEffect = DirtImpactEffect;
		break;
	case SURFACE_METAL:
		SelectedEffect = MetalImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}

	FVector ShotDirection = ImpactPoint - MuzzleComp->GetSocketLocation(MuzzleSocketName);
	ShotDirection.Normalize();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
		
	
	
}

void APBWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastFiredTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorld()->GetTimerManager().SetTimer(THandle_BetweenShots, this, &APBWeapon::Fire, TimeBetweenShots, true, FirstDelay);
	
}

void APBWeapon::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(THandle_BetweenShots);
}

void APBWeapon::PlayFireEffects_Implementation(FVector TraceEnd)
{
	
	if (MuzzleEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleEffect, MuzzleComp, MuzzleSocketName, FVector(0.f), FRotator(0.f), EAttachLocation::Type::SnapToTarget, true);
	}

	if (FireShotSound)
	{
		FSoundConcurrencySettings ConcurrencySettings;
		ConcurrencySettings.RetriggerTime = .01;
		ConcurrencySettings.MaxCount = 500;
		FireShotSound->bOverrideConcurrency = true;
		FireShotSound->ConcurrencyOverrides = ConcurrencySettings;
		
		if (GetLocalRole() < ROLE_Authority)
		{
			UGameplayStatics::SpawnSoundAttached(FireShotSound, MuzzleComp, MuzzleSocketName);
		}
		
	}


	if (TraceEffect)
	{
		UNiagaraComponent* traceComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceEffect, MeshComp->GetSocketLocation(MuzzleSocketName));
		if (traceComp)
		{
			traceComp->SetVectorParameter(TraceTargetName, TraceEnd);
			traceComp->SetVectorParameter(TraceStartName, MeshComp->GetSocketLocation(MuzzleSocketName));
		}
	}
	


	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientStartCameraShake(FireCamShake);
		}
	}
}

void APBWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APBWeapon, HitScanTrace, COND_SkipOwner);

}





