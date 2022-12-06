// Fill out your copyright notice in the Description page of Project Settings.


#include "PBWeaponProjectile.h"
#include "Engine/SkeletalMesh.h"
#include "Kismet/GameplayStatics.h"
#include "PBProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"


APBWeaponProjectile::APBWeaponProjectile()
{
	bShouldUseVolley = true;
	ShotsPerVolley = 8;
	Accuracy = 5.0f;
}

void APBWeaponProjectile::StartFire()
{
	float FirstDelay = FMath::Max(LastFiredTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorld()->GetTimerManager().SetTimer(THandle_BetweenShots, this, &APBWeaponProjectile::Fire, TimeBetweenShots, true, FirstDelay);
}

void APBWeaponProjectile::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(THandle_BetweenShots);
	if (CurrentProjectile)
	{
		ShotsFired = 0;
	}
	
}

void APBWeaponProjectile::Fire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();

	}
	
	if (ProjectileClass)
	{
		if (GetLocalRole() == ROLE_Authority)
		{
			if (bShouldUseVolley && ShotsFired < ShotsPerVolley)
			{
				FActorSpawnParameters spawnParams;
				spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				CurrentProjectile = (APBProjectile*)GetWorld()->SpawnActor<AActor>(ProjectileClass, MeshComp->GetSocketLocation(MuzzleSocketName), GetActorRotation(), spawnParams);
				CurrentProjectile->OwningWeapon = this;
				if (CurrentProjectile && TargetActor)
				{
					FVector TargetLocation = TargetActor->GetActorLocation();
					FVector MyLocation = GetActorLocation();
					double DistanceTo = (MyLocation - TargetLocation).Size();
					FVector TargetVelocity = TargetActor->GetVelocity();
					FVector temp = (DistanceTo / CurrentProjectile->ProjectileSpeed) * TargetVelocity;
					FVector ShotDisbursement = FMath::VRand() * (10000/(Accuracy*10));


					CurrentProjectile->ServerFireProjectile(this->Owner, this, BuildingActor, TargetActor, MeshComp->GetSocketLocation(MuzzleSocketName), TargetLocation + temp + ShotDisbursement);
					ShotsFired++;
				}
				FTimerHandle Delay;
				GetWorldTimerManager().SetTimer(Delay, this, &APBWeaponProjectile::Fire, 0.25f, false, 0.25f);
				
			}
			else if (!bShouldUseVolley)
			{
				FActorSpawnParameters spawnParams;
				spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				CurrentProjectile = (APBProjectile*)GetWorld()->SpawnActor<AActor>(ProjectileClass, MeshComp->GetSocketLocation(MuzzleSocketName), GetActorRotation(), spawnParams);
				CurrentProjectile->OwningWeapon = this;
				if (CurrentProjectile && TargetActor)
				{
					FVector TargetLocation = TargetActor->GetActorLocation();
					FVector MyLocation = GetActorLocation();
					double DistanceTo = (MyLocation - TargetLocation).Size();
					FVector TargetVelocity = TargetActor->GetVelocity();
					FVector temp = (DistanceTo / CurrentProjectile->ProjectileSpeed) * TargetVelocity;
					FVector ShotDisbursement = FMath::VRand() * (10000 / (Accuracy * 10));

					CurrentProjectile->ServerFireProjectile(this, this->Owner, BuildingActor, TargetActor, MeshComp->GetSocketLocation(MuzzleSocketName), TargetLocation + ShotDisbursement);
				}
			}
		}
		
		
		
	}
}


