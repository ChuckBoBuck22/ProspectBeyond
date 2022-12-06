// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PBPickupActor.generated.h"

class USphereComponent;
class APBPowerUpActor;

UCLASS()
class PB_API APBPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APBPickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	TSubclassOf<APBPowerUpActor> PowerUpClass;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* DecalComp;

	APBPowerUpActor* PowerUpInstance;

	UFUNCTION()
	void Respawn();

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	float CoolDownDuration;

	FTimerHandle RespawnTimer;

public:	
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
