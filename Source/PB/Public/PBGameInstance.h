// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PBGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PB_API UPBGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Instance")
	FName PlayerName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Instance")
	FString  PlayerPassword;

	UFUNCTION(BlueprintCallable)
	TArray<FString> GetAllMapNames();

protected:

private:
	

};
