// Fill out your copyright notice in the Description page of Project Settings.


#include "Networking/PBOnlineBeaconHostObject.h"
#include "Networking/PBOnlineBeaconClient.h"

APBOnlineBeaconHostObject::APBOnlineBeaconHostObject()
{
	ClientBeaconActorClass = APBOnlineBeaconClient::StaticClass();
	BeaconTypeName = ClientBeaconActorClass->GetName();

	Http = &FHttpModule::Get();
}

void APBOnlineBeaconHostObject::OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool success)
{
	if (success)
	{
		UE_LOG(LogTemp, Warning, TEXT("HttpRequest Success: %s"), *Response.Get()->GetContentAsString());
	}
	else
	{
		if (Response)
		{
			UE_LOG(LogTemp, Warning, TEXT("HttpRequest Failure: %s"), *Response.Get()->GetContentAsString());
		}
		
	}
}

void APBOnlineBeaconHostObject::OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection)
{
	Super::OnClientConnected(NewClientActor, ClientConnection);
	
	if (NewClientActor)
	{
		
		NewClientActor->SetBeaconOwner(this);
		ClientConnection->SetConnectionState(EConnectionState::USOCK_Open);
		UE_LOG(LogTemp, Warning, TEXT("Connected Client Valid"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Connected Client Invalid"));
	}

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject.Get()->SetStringField("world", "0");
	JsonObject.Get()->SetStringField("regionName", "Region Name");
	JsonObject.Get()->SetStringField("serverIp", "127.0.0.1");
	JsonObject.Get()->SetStringField("serverId", "");
	JsonObject.Get()->SetStringField("serverName", "Server Name");
	JsonObject.Get()->SetNumberField("activePlayers", 0);
	JsonObject.Get()->SetNumberField("maxPlayers", 100);
	JsonObject.Get()->SetStringField("id", "");

	FString JsonString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	TSharedRef<IHttpRequest> Request = Http->CreateRequest();

	Request.Get().OnProcessRequestComplete().BindUObject(this, &APBOnlineBeaconHostObject::OnProcessRequestComplete);

	Request.Get().SetURL("https://localhost:7185/ServerData/Create");
	Request.Get().SetVerb("Post");
	Request.Get().SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request.Get().SetContentAsString(JsonString);
	Request.Get().ProcessRequest();


}
