// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PBTurret.generated.h"



class APBProjectile;
class UPBHealthComponent;
class APBWeapon;
class APBBuilding;
class URotatingMovementComponent;
class UChildActorComponent;
/**
 * 
 */
UCLASS()
class PB_API APBTurret : public APawn
{
	GENERATED_BODY()
	
public:
	APBTurret();

	UFUNCTION(BlueprintCallable, Category = "Turret")
	void StartFire();
	UFUNCTION(BlueprintCallable, Category = "Turret")
	void StopFire();
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	URotatingMovementComponent* RotComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret")
	APBBuilding* Building;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Turret")
	APBWeapon* CurrentWeapon;

	virtual void Destroyed() override;

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Turret")
	TSubclassOf<APBBuilding> BuildingClass;

	UPROPERTY(EditDefaultsOnly, Category = "Turret")
	APBProjectile* Projectile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Turret")
	TSubclassOf<APBWeapon> TurretWeapon;

	
	UPBHealthComponent* HealthComp;
	

	UPROPERTY(VisibleDefaultsOnly, Category = "Turret")
	FName WeaponAttachSocketName;

	virtual void BeginPlay() override;



	
};
