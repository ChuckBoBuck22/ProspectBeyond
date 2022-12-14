// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PBHealthBarUI.generated.h"

/**
 * 
 */
UCLASS()
class PB_API UPBHealthBarUI : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Building;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite)
	AActor* BuildingOwner;

	
	
};
