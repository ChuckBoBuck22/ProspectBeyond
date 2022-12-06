// Fill out your copyright notice in the Description page of Project Settings.


#include "PBTurret.h"
#include "PBWeapon.h"
#include "PBPlayerState.h"
#include "PB/Public/Components/PBHealthComponent.h"
#include "PBWeaponProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "PBBuilding.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/RotatingMovementComponent.h"

APBTurret::APBTurret()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName("TurretMesh"));
	SetRootComponent(MeshComp);
	
	
	RotComp = CreateDefaultSubobject<URotatingMovementComponent>(FName("RotComp"));
	RotComp->bAutoActivate = false;
	RotComp->SetActive(false);
	RotComp->SetUpdatedComponent(MeshComp);
	RotComp->RotationRate = FRotator(0, 30, 0);

	bReplicates = true;

	WeaponAttachSocketName = "WeaponSocket";
}

void APBTurret::Destroyed()
{
	Super::Destroyed();
	DetachFromControllerPendingDestroy();
	RotComp->StopMovementImmediately();
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
	}
	
}

float APBTurret::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		UGameplayStatics::ApplyDamage(Building, Damage, EventInstigator, DamageCauser, DamageEvent.DamageTypeClass);
		return 0.0f;
	}
	
	return 0.0f;
}

void APBTurret::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		CurrentWeapon = GetWorld()->SpawnActor<APBWeapon>(TurretWeapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponSocket"));
		}
			
		Building = GetWorld()->SpawnActor<APBBuilding>(BuildingClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (Building)
		{
			if (Cast<APBWeaponProjectile>(CurrentWeapon)) 
			{
				Cast<APBWeaponProjectile>(CurrentWeapon)->BuildingActor = Building;
			}
			
			Building->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("MountSocket"));
			Building->Radius = 1;
			Building->Depth = 1;
			Building->OwningTurret = this;
		}
	}
}

void APBTurret::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void APBTurret::StopFire()
{
	CurrentWeapon->StopFire();
}


void APBTurret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBTurret, CurrentWeapon);

}