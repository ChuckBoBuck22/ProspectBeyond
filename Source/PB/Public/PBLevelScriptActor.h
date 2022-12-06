// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "PBLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class PB_API APBLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:

	APBLevelScriptActor();

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "ProspectBeyond")
	FString StationOwner;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "ProspectBeyond")
	FString StationName;
	
	
	
};
