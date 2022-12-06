// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconClient.h"
#include "PBOnlineBeaconClient.generated.h"


/**
 * 
 */
UCLASS()
class PB_API APBOnlineBeaconClient : public AOnlineBeaconClient
{
	GENERATED_BODY()

public:

	APBOnlineBeaconClient();

	/*UFUNCTION()
	bool ConnectToServer(FURL URL);*/

	UFUNCTION(BlueprintCallable)
		bool ConnectToServer(const FString& Address);

protected:
	
	virtual void OnFailure() override;
	virtual void OnConnected() override;
	FString DestinationAddress;
};
