// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PBHudUI.generated.h"

class APBPlayerState;
class APBLevelScriptActor;

/**
 * 
 */
UCLASS()
class PB_API UPBHudUI : public UUserWidget
{
	GENERATED_BODY()


public:
 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Prospect Beyond")
	APBLevelScriptActor* LevelScript;

	UFUNCTION(BlueprintCallable)
	FString	GetAreaName();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	APBPlayerState* PlayerStateHUDUI;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ToggleMenuMode();
	
};
