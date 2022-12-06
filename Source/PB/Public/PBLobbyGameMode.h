// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PBLobbyGameMode.generated.h"

class UPBLobbyUI;
class APBOnlineBeaconHostObject;
class UAgonesComponent;
/**
 * 
 */
UCLASS()
class PB_API APBLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APBLobbyGameMode();
	virtual void StartPlay() override;

protected:
	UFUNCTION(BlueprintCallable)
	bool CreateHostBeacon();
	
	APBOnlineBeaconHostObject* HostObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAgonesComponent* AgonesSDK;
};
