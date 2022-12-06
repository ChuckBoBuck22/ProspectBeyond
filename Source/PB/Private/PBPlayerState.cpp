// Fill out your copyright notice in the Description page of Project Settings.


#include "PBPlayerState.h"
#include "PBCommand.h"
#include "Net/UnrealNetwork.h"


APBPlayerState::APBPlayerState()
{
	bReplicates = true;
	Team = 16;
	Faction = EPBFaction::EF_NoTeam;
}

void APBPlayerState::AddStation(APBCommand* stationToAdd)
{
	OwnedStations.Add(stationToAdd);
}

void APBPlayerState::RemoveStation(APBCommand* stationToRemove)
{
	OwnedStations.Remove(stationToRemove);
}

void APBPlayerState::Update()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		if (!OwnedStations.IsEmpty())
		{
			float A_Stores = 0, B_Stores = 0, C_Stores = 0,
				A_Prod = 0, B_Prod = 0, C_Prod = 0;
			for (int i = 0; i < OwnedStations.Num(); i++)
			{
				if (OwnedStations[i] != NULL && OwnedStations[i]->IsActorInitialized() && !OwnedStations[i]->StationName.IsEmpty())
				{
					A_Stores += OwnedStations[i]->ResourceA_Stores;
					B_Stores += OwnedStations[i]->ResourceB_Stores;
					C_Stores += OwnedStations[i]->ResourceC_Stores;
					A_Prod += OwnedStations[i]->ResourceA_Production;
					B_Prod += OwnedStations[i]->ResourceB_Production;
					C_Prod += OwnedStations[i]->ResourceC_Production;
				}
				
			}
			ResourceA_Stores = A_Stores;
			ResourceB_Stores = B_Stores;
			ResourceC_Stores = C_Stores;
			ResourceA_Production = A_Prod;
			ResourceB_Production = B_Prod;
			ResourceC_Production = C_Prod;
		}
	}
	
}

void APBPlayerState::BeginPlay()
{
	FTimerHandle timerHandle;
	GetWorldTimerManager().SetTimer(timerHandle, this, &APBPlayerState::Update, 2, true);
	SetAreaLevelName();
	Team = 16;
}

void APBPlayerState::SetAreaName(FString name)
{
	if (this && !name.IsEmpty()) 
	{
		AreaName = name;
	}
	
}

void APBPlayerState::SetAreaLevelName()
{
	if (this)
	{
		if (GetLocalRole() < ROLE_Authority)
		{
			APawn* playerPawn = GetPawn();
			if (playerPawn)
			{
				SetAreaName(playerPawn->GetLevel()->GetOuter()->GetFName().ToString());
				return;
			}
			FTimerHandle timerHandle;
			GetWorldTimerManager().SetTimer(timerHandle, this, &APBPlayerState::SetAreaLevelName, 5, false, 5);
		}
	}
}

uint8 APBPlayerState::GetTeam()
{
	return Team;
}

void APBPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBPlayerState, ResourceA_Production);
	DOREPLIFETIME(APBPlayerState, ResourceB_Production);
	DOREPLIFETIME(APBPlayerState, ResourceC_Production);
	DOREPLIFETIME(APBPlayerState, ResourceA_Stores);
	DOREPLIFETIME(APBPlayerState, ResourceB_Stores);
	DOREPLIFETIME(APBPlayerState, ResourceC_Stores);
	DOREPLIFETIME(APBPlayerState, OwnedStations);
	DOREPLIFETIME(APBPlayerState, LastCharacterLocation);
	DOREPLIFETIME(APBPlayerState, LastOwnedCommandVisited);
	DOREPLIFETIME(APBPlayerState, AreaName);
	DOREPLIFETIME(APBPlayerState, Faction);


}





