// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PBGameMode.generated.h"

class APBOnlineBeaconHostObject;

UCLASS(minimalapi)
class APBGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APBGameMode();
	virtual void StartPlay() override;
	virtual void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;
	

protected:
	
	virtual void OnPostLogin(AController* NewPlayer) override;

	UFUNCTION(BlueprintCallable)
	bool CreateHostBeacon();

	APBOnlineBeaconHostObject* HostObject;
	
};



