// Fill out your copyright notice in the Description page of Project Settings.


#include "PBProjectile.h"
#include "PB.h"
#include "PBCharacter.h"
#include "PBWeaponProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


// Sets default values
APBProjectile::APBProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMesh->SetWorldScale3D(FVector(10.0f, 10.0f, 10.0f));
	RootComponent = ProjectileMesh;

	ProjectileMesh->SetSimulatePhysics(false);
	ProjectileSpeed = 5000;
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjectileMovementComponent->Velocity = FVector(0.f, 0.f, 0.f);
	ProjectileMovementComponent->InitialSpeed = ProjectileSpeed;
	ProjectileMovementComponent->MaxSpeed = ProjectileSpeed + 1000;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->SetUpdatedComponent(ProjectileMesh);

	bReplicates = true;
	
	
	
}

// Called when the game starts or when spawned
void APBProjectile::BeginPlay()
{
	Super::BeginPlay();

	/* Disabling Timer to test explode on impact*/
	/*FTimerHandle timerHandle;
	GetWorldTimerManager().SetTimer(timerHandle, this, &APBProjectile::Explode, 2);*/
	
	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &APBProjectile::Explode);
	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &APBProjectile::PlayBounceEffects);
	OnDestroyed.AddDynamic(this, &APBProjectile::PlayDestroyEffects);
	
}

void APBProjectile::PlayDestroyEffects(AActor* DestroyedActor)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionEffect, DestroyedActor->GetActorLocation(), FRotator(0.f), FVector(3.f));
		
		
		UGameplayStatics::SpawnSoundAttached(ExplodeSound, RootComponent);
	}
}

void APBProjectile::MulticastPlayFireShotEffects_Implementation()
{
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), FireShotSound, GetActorLocation());
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FireShotEffect, GetActorLocation());
}

void APBProjectile::Explode(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (GetLocalRole() == ROLE_Authority && (Bounces > 3 || ImpactResult.GetActor()->IsA<APBCharacter>()))
	{
		UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), BaseDamage, MinimumDamage, ImpactResult.Location, InnerRadius, OuterRadius, FalloffRate, DamageTypeClass, IgnoreActors);
		Destroy();
	}
	else
	{
		Bounces++;
	}
	
}

void APBProjectile::PlayBounceEffects(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (GetLocalRole() < ROLE_Authority && Bounces < 3)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), BounceSound, ImpactResult.Location);
	}
}

// Called every frame
void APBProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APBProjectile::ServerFireProjectile_Implementation(AActor* TurretActor, AActor* WeaponActor, AActor* BuildingActor, AActor* TargetActor, FVector StartLocation, FVector TargetLocation)
{
	IgnoreActors.Add(TurretActor);
	IgnoreActors.Add(WeaponActor);
	IgnoreActors.Add(BuildingActor);
	IgnoreActors.Add(this);
	FVector TossVelocity;
	UGameplayStatics::SuggestProjectileVelocity(GetWorld(), TossVelocity, StartLocation, TargetLocation, ProjectileSpeed, true, 50.f, 0.f, ESuggestProjVelocityTraceOption::OnlyTraceWhileAscending, ECollisionResponse::ECR_Block, IgnoreActors, false);
	if (WeaponActor)
	{
		Cast<APBWeaponProjectile>(OwningWeapon)->MeshComp->SetRelativeRotation(FRotator(TossVelocity.Rotation().Pitch, 0.f, 0.f));
	}
	ProjectileMovementComponent->Velocity = TossVelocity;
	MulticastPlayFireShotEffects();
}

bool APBProjectile::ServerFireProjectile_Validate(AActor* TurretActor, AActor* WeaponActor, AActor* BuildingActor, AActor* TargetActor, FVector StartLocation, FVector TargetLocation)
{
	return true;
}

void APBProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBProjectile, ProjectileMesh);
	

}
