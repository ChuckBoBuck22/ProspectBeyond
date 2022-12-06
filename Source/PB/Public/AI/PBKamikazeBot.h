// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PBKamikazeBot.generated.h"

class UPBHealthComponent;
class UNiagaraSystem;
class USphereComponent;
class USoundCue;

UCLASS()
class PB_API APBKamikazeBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APBKamikazeBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	FVector GetNextPathPoint();

	FVector NextPathPoint;
	UPROPERTY(EditDefaultsOnly, Category = "KamikazeBot")
	float MovementForce;
	UPROPERTY(EditDefaultsOnly, Category = "KamikazeBot")
	bool bUseVelocityChange;
	UPROPERTY(EditDefaultsOnly, Category = "KamikazeBot")
	float RequiredDistanceToTarget;
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UPBHealthComponent* HealthComponent;
	UFUNCTION()
	void HandleTakeDamage(UPBHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY()
	UMaterialInstanceDynamic* MatInst;

	void SelfDestruct();

	UPROPERTY(EditDefaultsOnly, Category = "KamikazeBot")
	UNiagaraSystem* ExplosionEffect;

	bool bExploded;

	UPROPERTY(EditDefaultsOnly, Category = "KamikazeBot")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "KamikazeBot")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "KamikazeBot")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, Category = "KamikazeBot")
	USphereComponent* SphereComponent;

	FTimerHandle TimerHandle_SelfDamage;

	float SelfDamageInterval;

	UFUNCTION()
	void DamageSelf();

	bool bStartedSelfDestruct;

	UPROPERTY(EditDefaultsOnly, Category = "KamikazeBot")
	USoundCue* SelfDestructSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "KamikazeBot")
	USoundCue* ExplodeSound;

	
	void CheckProximity();

	UFUNCTION()
	void UpdateMaterial(float Alpha);


	UPROPERTY(EditDefaultsOnly, Category = "KamikazeBot")
	int32 PowerLevel;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	

};
