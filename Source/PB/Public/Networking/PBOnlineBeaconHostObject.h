// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconHostObject.h"
#include "Http.h"
#include "PBOnlineBeaconHostObject.generated.h"

/**
 * 
 */
UCLASS()
class PB_API APBOnlineBeaconHostObject : public AOnlineBeaconHostObject
{
	GENERATED_BODY()
	

public:

	APBOnlineBeaconHostObject();

protected:
	FHttpModule* Http;

	void OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool success);

	virtual void OnClientConnected(AOnlineBeaconClient* NewClientActor, UNetConnection* ClientConnection) override;
};
