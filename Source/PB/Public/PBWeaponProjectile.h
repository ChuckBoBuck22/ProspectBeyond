// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PBWeapon.h"
#include "PBWeaponProjectile.generated.h"


class APBProjectile;
/**
 * 
 */
UCLASS()
class PB_API APBWeaponProjectile : public APBWeapon
{
	GENERATED_BODY()
		

public:
	APBWeaponProjectile();
	virtual void StartFire() override;

	virtual void StopFire() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ProjectileWeapon")
	AActor* TargetActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ProjectileWeapon")
	AActor* TurretActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ProjectileWeapon")
	AActor* BuildingActor;

protected:
	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileWeapon")
	TSubclassOf<AActor> ProjectileClass;

	APBProjectile* CurrentProjectile;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileWeapon")
	int ShotsFired;
	UPROPERTY(EditDefaultsOnly, Category = "ProjectileWeapon")
	int ShotsPerVolley;
	UPROPERTY(EditDefaultsOnly, Category = "ProjectileWeapon")
	bool bShouldUseVolley;
	UPROPERTY(EditDefaultsOnly, Category = "ProjectileWeapon", meta = (ClampMin = 0.1f, ClampMax = 10.0f))
	float Accuracy;
	
};
