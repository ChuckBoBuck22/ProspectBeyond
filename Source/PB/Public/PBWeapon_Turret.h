// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PBWeapon.h"
#include "PBWeapon_Turret.generated.h"


class APBTurret;
/**
 * 
 */
UCLASS()
class PB_API APBWeapon_Turret : public APBWeapon
{
	GENERATED_BODY()

protected:

	

public:
	
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
