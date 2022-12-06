// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PBBuilding.h"
#include "PBResBldng.generated.h"


class ADecalActor;
/**
 * 
 */
UCLASS()
class PB_API APBResBldng : public APBBuilding
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "PB")
	float ResourceAProduction = 5;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "PB")
	float ResourceBProduction = 7;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "PB")
	float ResourceCProduction = 9;

	APBResBldng();

	virtual void BeginPlay() override;


	
};
