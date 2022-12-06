// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "PBBuildPawn.generated.h"


class UInputMappingContext;
class UInputAction;
class APBCommand;
/**
 * 
 */
UCLASS()
class PB_API APBBuildPawn : public ADefaultPawn
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MenuModeAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* PlaceBuildingAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* PlaceBuildingEscapeAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ExitBuildModeAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* InputMappingContextBuildMode;

	
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PawnClientRestart() override;
	void PlaceBuildingInputAction();
	void PlaceBuildingEscapeInputAction();
	void ToggleMenuMode();
	void ExitBuildMode();

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
	APBCommand* CurrentCommand;
};
