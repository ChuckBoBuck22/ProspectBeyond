// Fill out your copyright notice in the Description page of Project Settings.


#include "PBLobbyGameMode.h"
#include "Blueprint/UserWidget.h"
#include "PBLobbyUI.h"
#include "PBPlayerController.h"
#include "OnlineBeaconHost.h"
#include "Networking/PBOnlineBeaconHostObject.h"
#include "Kismet/KismetStringLibrary.h"
#include "AgonesComponent.h"

APBLobbyGameMode::APBLobbyGameMode()
{
	HostObject = nullptr;
	AgonesSDK = CreateDefaultSubobject<UAgonesComponent>(TEXT("AgonesSDK"));
}

void APBLobbyGameMode::StartPlay()
{
	Super::StartPlay();

}

bool APBLobbyGameMode::CreateHostBeacon()
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
				UE_LOG(LogTemp, Warning, TEXT("Net Driver Name (Host) on Port: %s"), *Host->GetNetDriverName().ToString());
				UE_LOG(LogTemp, Warning, TEXT("Net Driver Name (HostObject) on Port: %s"), *HostObject->GetNetDriverName().ToString());
				UE_LOG(LogTemp, Warning, TEXT("Host Registered on Port: %s"), *UKismetStringLibrary::Conv_IntToString(Host->ListenPort));
				return true;
			}
		}
	}
	return false;
}


