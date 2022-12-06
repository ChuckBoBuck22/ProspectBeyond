// Fill out your copyright notice in the Description page of Project Settings.


#include "PBBuildPawn.h"
#include "EnhancedInputComponent.h"
#include <EnhancedInput/Public/EnhancedInputSubsystems.h>
#include "PBPlayerController.h"

void APBBuildPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		PlayerEnhancedInputComponent->BindAction(MenuModeAction, ETriggerEvent::Triggered, this, &APBBuildPawn::ToggleMenuMode);
		PlayerEnhancedInputComponent->BindAction(PlaceBuildingAction, ETriggerEvent::Triggered, this, &APBBuildPawn::PlaceBuildingInputAction);
		PlayerEnhancedInputComponent->BindAction(PlaceBuildingEscapeAction, ETriggerEvent::Triggered, this, &APBBuildPawn::PlaceBuildingEscapeInputAction);
		PlayerEnhancedInputComponent->BindAction(ExitBuildModeAction, ETriggerEvent::Triggered, this, &APBBuildPawn::ExitBuildMode);

		PlayerInputComponent->BindAxis("Move Forward / Backward", this, &APBBuildPawn::MoveForward);
		PlayerInputComponent->BindAxis("Move Right / Left", this, &APBBuildPawn::MoveRight);

		// We have 2 versions of the rotation bindings to handle different kinds of devices differently
		// "turn" handles devices that provide an absolute delta, such as a mouse.
		// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
		PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
		PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &APBBuildPawn::TurnAtRate);
		PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
		PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &APBBuildPawn::LookUpAtRate);
	}
}

void APBBuildPawn::PawnClientRestart()
{
	Super::PawnClientRestart();
	// Make sure that we have a valid PlayerController.
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// PawnClientRestart can run more than once in an Actor's lifetime, so start by clearing out any leftover mappings.
			Subsystem->ClearAllMappings();

			// Add each mapping context, along with their priority values. Higher values outprioritize lower values.
			Subsystem->AddMappingContext(InputMappingContextBuildMode, 1);
		}
	}
}

void APBBuildPawn::PlaceBuildingInputAction()
{
	APBPlayerController* PC = Cast<APBPlayerController>(GetController());
	PC->isMousePressed();
}

void APBBuildPawn::PlaceBuildingEscapeInputAction()
{
	APBPlayerController* PC = Cast<APBPlayerController>(GetController());
	PC->isRightMousePressed();
}

void APBBuildPawn::ToggleMenuMode()
{
	APBPlayerController* PC = Cast<APBPlayerController>(GetController());
	PC->ToggleMenuMode();
}

void APBBuildPawn::ExitBuildMode()
{
	APBPlayerController* PC = Cast<APBPlayerController>(GetController());
	PC->ServerExitBuildMode();
}
