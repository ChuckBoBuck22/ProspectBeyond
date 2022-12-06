// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PBWeaponProjectile.h"
#include "PBWeaponProjectile_Turret.generated.h"


class APBTurret;
/**
 * 
 */
UCLASS()
class PB_API APBWeaponProjectile_Turret : public APBWeaponProjectile
{
	GENERATED_BODY()
	
protected:
	

public:
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
