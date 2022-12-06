// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PBFaction.h"
#include "PBPlayerState.generated.h"


/**
 * 
 */
UCLASS()
class PB_API APBPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	APBPlayerState();

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	FString AreaName;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	float ResourceA_Production;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	float ResourceB_Production;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	float ResourceC_Production;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	float ResourceA_Stores;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	float ResourceB_Stores;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	float ResourceC_Stores;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	TArray<APBCommand*> OwnedStations;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	FVector LastCharacterLocation;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	APBCommand* LastOwnedCommandVisited;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Teams")
	uint8 Team;

	UFUNCTION(BlueprintCallable)
	void AddStation(APBCommand* stationToAdd);

	UFUNCTION(BlueprintCallable)
	void RemoveStation(APBCommand* stationToRemove);

	UFUNCTION()
	void Update();

	UFUNCTION(BlueprintCallable)
	void SetAreaName(FString name);

	UFUNCTION(BlueprintCallable)
	void SetAreaLevelName();

	uint8 GetTeam();
	UFUNCTION(BlueprintCallable, Category = "Teams")
	FORCEINLINE EPBFaction GetFaction() const { return Faction; }
	FORCEINLINE void SetFaction(EPBFaction FactionToSet) { Faction = FactionToSet; }

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(Replicated, VisibleAnywhere)
	EPBFaction Faction;
	
	
};
