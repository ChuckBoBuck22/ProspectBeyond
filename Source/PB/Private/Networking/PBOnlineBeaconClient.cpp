// Fill out your copyright notice in the Description page of Project Settings.


#include "Networking/PBOnlineBeaconClient.h"
#include "Kismet/KismetStringLibrary.h"
#include "PBPlayerController.h"

APBOnlineBeaconClient::APBOnlineBeaconClient()
{
}

//bool APBOnlineBeaconClient::ConnectToServer(FURL URL)
//{
//	return InitClient(URL);
//
//}

bool APBOnlineBeaconClient::ConnectToServer(const FString& Address)
{
	DestinationAddress = Address;
	FURL Destination = FURL(nullptr, *FString("127.0.0.1"), ETravelType::TRAVEL_Absolute);
	Destination.Port = UKismetStringLibrary::Conv_StringToInt(Address.Right(4)) + 100;
	return InitClient(Destination);
}

void APBOnlineBeaconClient::OnFailure()
{
	Super::OnFailure();
	UE_LOG(LogTemp, Warning, TEXT("Client Failed to Connect to Host Beacon"));
	Cast<APBPlayerController>(GetWorld()->GetFirstPlayerController())->ServerCreateServer(DestinationAddress);
	Cast<APBPlayerController>(GetWorld()->GetFirstPlayerController())->Travel(DestinationAddress);
	this->DestroyBeacon();
}

void APBOnlineBeaconClient::OnConnected()
{
	Super::OnConnected();
	BeaconState = EBeaconState::AllowRequests;
	Cast<APBPlayerController>(GetWorld()->GetFirstPlayerController())->ExecuteTravel(DestinationAddress);
	UE_LOG(LogTemp, Warning, TEXT("Client Connected to Host Beacon"));
	this->DestroyBeacon();
}
