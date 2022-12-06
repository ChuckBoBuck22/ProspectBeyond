// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PBBuilding.h"
#include "PBDepot.generated.h"

UCLASS()
class PB_API APBDepot : public APBBuilding
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APBDepot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Depot")
	int DepotLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Depot")
	float ResourceA_Stores;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Depot")
	float ResourceB_Stores;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Depot")
	float ResourceC_Stores;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
