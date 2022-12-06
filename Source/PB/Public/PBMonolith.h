// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PBMonolith.generated.h"

class UBoxComponent;
class APBCommand;

UCLASS()
class PB_API APBMonolith : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APBMonolith();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Monolith")
	UStaticMeshComponent* MeshComp;
	UPROPERTY(VisibleAnywhere, Category = "Monolith")
	UBoxComponent* Area;
	UPROPERTY(VisibleAnywhere, Category = "Monolith")
	TArray<AActor*> Stations;
	UPROPERTY(VisibleAnywhere, Category = "Monolith")
	APBCommand* DesignatedStation;
	UPROPERTY(VisibleAnywhere, Category = "Monolith")
	FTimerHandle DesignationHandle;
	UPROPERTY(VisibleAnywhere, Category = "Monolith")
	TArray<APBCommand*> Depots;
	//UPROPERTY(EditDefaultsOnly, Category = "Monolith")
	//TODO: ADD ENUM For TYPE

	APBCommand* DesignateStation();
	void Designate();
};
