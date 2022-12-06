// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PBBuilding.h"
#include "PBCommand.generated.h"

/**
 * 
 */

class UPBHealthComponent;
class UWidgetComponent;
class UPBDIalogueUI;


UCLASS()
class PB_API APBCommand : public APBBuilding
{
	GENERATED_BODY()

	APBCommand();
	void AddDialogueUI();
	void Update();

	UPROPERTY(EditAnywhere)
	class UBoxComponent* WallOne;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* WallTwo;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* WallThree;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* WallFour;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* WallFive;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* TriggerArea;

public:

	UFUNCTION(BlueprintCallable)
	void SetStationName(FString _StationName);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<APBBuilding*> Buildings;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bHasDepot;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float ResourceA_Production;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float ResourceB_Production;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float ResourceC_Production;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float ResourceA_Stores;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float ResourceB_Stores;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float ResourceC_Stores;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	FString StationName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsHomeStation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Station")
	bool bIsDesignated;

	UFUNCTION()
	void TriggerAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void TriggerAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OwnedAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OwnedAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void AddBuilding(APBBuilding* buildingToAdd);

	UFUNCTION(BlueprintCallable)
	void RemoveBuilding(APBBuilding* buildingToRemove);
	//UFUNCTION(BlueprintCallable)
	//void UpdateAreaNamesOnNew();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TSubclassOf<UUserWidget> DialogueTemp;

	UPROPERTY()
	UPBDIalogueUI* DialogueUI;
	
};
