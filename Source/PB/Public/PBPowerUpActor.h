// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PBPowerUpActor.generated.h"

UCLASS()
class PB_API APBPowerUpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APBPowerUpActor();

protected:

	UPROPERTY(ReplicatedUsing=OnRep_PowerUpActive)
	bool bIsPowerUpActive;

	UFUNCTION()
	void OnRep_PowerUpActive();

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float PowerUpInterval;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 TotalNumOfTicks;

	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 TicksProcessed;

	FTimerHandle TimerHandlePowerUpTick;

	UFUNCTION()
	void OnTickPowerUp();

	

public:	

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerUpStateChange(bool bNewIsActive);

	void ActivatePowerUp(AActor* ActivatedActor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnActivated(AActor* ActivatedPlayer);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerUpTicked();


};
