// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PBProjectile.generated.h"



class UProjectileMovementComponent;
class UStaticMeshComponent;
class UNiagaraSystem;
class USoundCue;

UCLASS()
class PB_API APBProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APBProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	int Bounces;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "PB Projectile")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PB Projectile")
	UNiagaraSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "PB Projectile")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, Category = "PB Projectile")
	float MinimumDamage;

	UFUNCTION()
	void PlayDestroyEffects(AActor* DestroyedActor);

	UPROPERTY(EditDefaultsOnly, Category = "PB Projectile")
	float OuterRadius;
	
	UPROPERTY(EditDefaultsOnly, Category = "PB Projectile")
	float FalloffRate;

	UPROPERTY(EditDefaultsOnly, Category = "PB Projectile")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditDefaultsOnly, Category = "PB Projectile")
	TArray<AActor*> IgnoreActors;

	UPROPERTY(EditDefaultsOnly, Category = "PB Projectile")
	USoundCue* ExplodeSound;

	UPROPERTY(EditDefaultsOnly, Category = "PB Projectile")
	USoundCue* BounceSound;

	UPROPERTY(EditDefaultsOnly, Category = "PB Projectile")
	USoundCue* FireShotSound;

	UPROPERTY(EditDefaultsOnly, Category = "PB Projectile")
	UNiagaraSystem* FireShotEffect;

	UFUNCTION(NetMulticast, reliable)
	void MulticastPlayFireShotEffects();

	UFUNCTION(BlueprintCallable)
	void Explode(const FHitResult& ImpactResult, const FVector& ImpactVelocity);
	UFUNCTION()
	void PlayBounceEffects(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "PB Projectile")
	AActor* OwningWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "PB Projectile")
	float ProjectileSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "PB Projectile")
	float InnerRadius;
	UPROPERTY(VisibleAnywhere, Category = "PB Projectile")
	FVector InitVelocity;
	
	UPROPERTY(EditDefaultsOnly, Category = "PB Projectile")
	UProjectileMovementComponent* ProjectileMovementComponent;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFireProjectile(AActor* TurretActor, AActor* WeaponActor, AActor* BuildingActor, AActor* TargetActor, FVector StartLocation, FVector TargetLocation);
};
