// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PBGameStateBase.generated.h"

class APBPlayerState;
class ADecalActor;
/**
 * 
 */
UCLASS()
class PB_API APBGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	TArray<APBPlayerState*> wildphire;
	TArray<APBPlayerState*> AlphaAspect;
	TArray<APBPlayerState*> proclout;
	TArray<APBPlayerState*> SenseDynamic;
	TArray<APBPlayerState*> SportingPower;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameState")
	ADecalActor* ResourceDecal;

	UPROPERTY(ReplicatedUsing = OnRep_wildphireScore)
	float wildphireScore = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_AlphaAspectScore)
	float AlphaAspectScore = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_procloutScore)
	float procloutScore = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_SenseDynamicScore)
	float SenseDynamicScore = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_SportingPowerScore)
	float SportingPowerScore = 0.f;

	UFUNCTION()
	void OnRep_wildphireScore();

	UFUNCTION()
	void OnRep_AlphaAspectScore();

	UFUNCTION()
	void OnRep_procloutScore();

	UFUNCTION()
	void OnRep_SenseDynamicScore();

	UFUNCTION()
	void OnRep_SportingPowerScore();
	
};
