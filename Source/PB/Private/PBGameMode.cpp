// Copyright Epic Games, Inc. All Rights Reserved.

#include "PBGameMode.h"
#include "PBCharacter.h"
#include "PBGameStateBase.h"
#include "PBPlayerState.h"
#include "PBPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Networking/PBOnlineBeaconHostObject.h"
#include "OnlineBeaconHost.h"
#include "Kismet/KismetStringLibrary.h"

APBGameMode::APBGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("Blueprint'/Game/Blueprints/Character/B_PBCharacter'"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	bUseSeamlessTravel = true;
	
}

void APBGameMode::StartPlay()
{
	Super::StartPlay();
	
	APBGameStateBase* PBGameState = GetGameState<APBGameStateBase>();
	if (PBGameState)
	{

		for (auto PState : PBGameState->PlayerArray)
		{
			APBPlayerState* PlayerState = Cast<APBPlayerState>(PState);
			if (PlayerState && PlayerState->GetFaction() == EPBFaction::EF_NoTeam)
			{

				int index = 0;
				int sizes[5] = {
					PBGameState->wildphire.Num(),
					PBGameState->AlphaAspect.Num(),
					PBGameState->proclout.Num(),
					PBGameState->SenseDynamic.Num(),
					PBGameState->SportingPower.Num() };
				int shortest = sizes[0];
				for (int i = 1; i < 5; i++)
				{
					if (sizes[i] < shortest)
					{
						shortest = sizes[i];
						index = i;
					}
				}
				PlayerState->SetFaction(EPBFaction(index));
				switch (index)
				{
				case 0:
					PBGameState->wildphire.AddUnique(PlayerState);
					break;
				case 1:
					PBGameState->AlphaAspect.AddUnique(PlayerState);
					break;
				case 2:
					PBGameState->proclout.AddUnique(PlayerState);
					break;
				case 3:
					PBGameState->SenseDynamic.AddUnique(PlayerState);
					break;
				case 4:
					PBGameState->SportingPower.AddUnique(PlayerState);
					break;
				}
			}
		}
		
	}
}

void APBGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	APBGameStateBase* PBGameState = GetGameState<APBGameStateBase>();
	if (PBGameState)
	{
		
		APBPlayerState* PlayerState = NewPlayer->GetPlayerState<APBPlayerState>();
		if (PlayerState && PlayerState->GetFaction() == EPBFaction::EF_NoTeam)
		{
			
			int index = 0;
			int sizes[5] = { 
				PBGameState->wildphire.Num(),
				PBGameState->AlphaAspect.Num(),
				PBGameState->proclout.Num(),
				PBGameState->SenseDynamic.Num(),
				PBGameState->SportingPower.Num() };
			int shortest = sizes[0];
			for (int i = 1; i < 5; i++)
			{
				if (sizes[i] < shortest)
				{
					shortest = sizes[i];
					index = i;
				}
			}
			PlayerState->SetFaction(EPBFaction(index));
			switch (index)
			{
			case 0: 
				PBGameState->wildphire.AddUnique(PlayerState);
				break;
			case 1:
				PBGameState->AlphaAspect.AddUnique(PlayerState);
				break;
			case 2:
				PBGameState->proclout.AddUnique(PlayerState);
				break;
			case 3:
				PBGameState->SenseDynamic.AddUnique(PlayerState);
				break;
			case 4:
				PBGameState->SportingPower.AddUnique(PlayerState);
				break;
			}
		}
		if (PlayerState)
		{
			NewPlayer->SetPawn(PlayerState->GetPawn());
		}
	}
}

bool APBGameMode::CreateHostBeacon()
{
	if (AOnlineBeaconHost* Host = GetWorld()->SpawnActor<AOnlineBeaconHost>(AOnlineBeaconHost::StaticClass()))
	{
		Host->ListenPort = GetWorld()->URL.Port + 100;
		if (Host->InitHost())
		{
			Host->PauseBeaconRequests(false);
			HostObject = GetWorld()->SpawnActor<APBOnlineBeaconHostObject>(APBOnlineBeaconHostObject::StaticClass());
			if (HostObject)
			{
				
				//HostObject->SetNetDriverName(FName("HostBeacon" + (GetWorld()->URL.Port + 100)));
				
				//HostObject->GetNetConnection()->RequestURL = "127.0.0.1:" + UKismetStringLibrary::Conv_IntToString(GetWorld()->URL.Port + 100);
				Host->RegisterHost(HostObject);
				UE_LOG(LogTemp, Warning, TEXT("Host Registered on Port: %s"), *Host->GetNetDriverName().ToString());
				UE_LOG(LogTemp, Warning, TEXT("Host Registered on Port: %s"), *HostObject->GetNetDriverName().ToString());
				UE_LOG(LogTemp, Warning, TEXT("Host Registered on Port: %s"), *UKismetStringLibrary::Conv_IntToString(Host->ListenPort));
				return true;
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Host Failed to Register"));
	return false;
}

void APBGameMode::RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot)
{
	
	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
	{
		return;
	}

	if (!StartSpot)
	{
		UE_LOG(LogGameMode, Warning, TEXT("RestartPlayerAtPlayerStart: Player start not found"));
		return;
	}

	FRotator SpawnRotation = StartSpot->GetActorRotation();

	UE_LOG(LogGameMode, Verbose, TEXT("RestartPlayerAtPlayerStart %s"), (NewPlayer && NewPlayer->PlayerState) ? *NewPlayer->PlayerState->GetPlayerName() : TEXT("Unknown"));

	if (MustSpectate(Cast<APlayerController>(NewPlayer)))
	{
		UE_LOG(LogGameMode, Verbose, TEXT("RestartPlayerAtPlayerStart: Tried to restart a spectator-only player!"));
		return;
	}

	if (Cast<APBPlayerController>(NewPlayer)->GetPawn() != nullptr)
	{
		// If we have an existing pawn, just use it's rotation
		SpawnRotation = NewPlayer->GetPawn()->GetActorRotation();
	}
	else if (GetDefaultPawnClassForController(NewPlayer) != nullptr)
	{
		// Try to create a pawn to use of the default class for this player
		APawn* NewPawn = SpawnDefaultPawnFor(NewPlayer, StartSpot);
		if (IsValid(NewPawn))
		{
			NewPlayer->SetPawn(NewPawn);
		}
	}

	if (!IsValid(NewPlayer->GetPawn()))
	{
		FailedToRestartPlayer(NewPlayer);
	}
	else
	{
		// Tell the start spot it was used
		InitStartSpot(StartSpot, NewPlayer);

		FinishRestartPlayer(NewPlayer, SpawnRotation);
	}
}
