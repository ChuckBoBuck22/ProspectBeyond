// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PBPlayerController.generated.h"

class APBCommand;
class APBBuildPawn;
class UPBDIalogueUI;
class UPBHudUI;
class APBCharacter;
class UPBLobbyUI;
class APBOnlineBeaconClient;
/**
 * 
 */
UCLASS()
class PB_API APBPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	APBCharacter* CharacterBP;

	UPROPERTY()
	TSubclassOf<APBCharacter> CharacterTemp;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> GhostBuilding;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> BuildingToPlace;

	UPROPERTY()
	AActor* GhostActor;

	UPROPERTY()
	AActor* LocalCommand;

	UPROPERTY()
	bool bPlaced;

	UPROPERTY(Replicated)
	AActor* ActualActor;

	UPROPERTY()
	FTimerHandle TimerHandle;

	UPROPERTY()
	FVector Location;
	UFUNCTION(BlueprintCallable)
	void EnterBuildMode(APBCommand* command);
	UFUNCTION(BlueprintCallable)
	void PlaceBuilding();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPlaceBuilding(TSubclassOf<AActor> building, FVector loc, FRotator rot);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCreateServer(const FString& DestinationAddress);

	UFUNCTION()
	void AddDialogueUI(AActor* actor);

	void ToggleMenuMode();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerExitBuildMode();

	UFUNCTION(Client, Reliable)
	void ClientExitBuildMode();

	UFUNCTION(BlueprintCallable)
	void Travel(FString Name);

	void TravelHelper(APBOnlineBeaconClient* BeaconClient, FString URLString, FURL URL, FString Name, int count, bool ServerCreated);
	void ExecuteTravel(FString Destination);

	void isMousePressed();
	void isRightMousePressed();
	void UpdateLocation();
	APBPlayerController();
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	void ToggleResourceDecal();
	void SetHudUIPlayerState();
	void AddWidgetToViewport(UUserWidget* WidgetToAdd);

private:
	void SetupHUD();
	virtual void PostSeamlessTravel() override;

	UPROPERTY()
	bool bPlacingBuilding;

	UPROPERTY(Replicated)
	bool bInBuildMode;

	UPROPERTY()
	TSubclassOf<APBBuildPawn> BuildPawnTemp;

	UPROPERTY()
	APBBuildPawn* BuildPawn;

	UPROPERTY()
	UPBDIalogueUI* DialogueUI;

	UPROPERTY()
	UPBHudUI* HudUI;

	UPROPERTY()
	TSubclassOf<UUserWidget> DialogueTemp;

	UPROPERTY()
	TSubclassOf<UUserWidget> HudTemp;

	UPROPERTY()
	TSubclassOf<APBCommand> CommandTemp;
	
	UPROPERTY()
	UPBLobbyUI* LobbyUI;

	UPROPERTY()
	TSubclassOf<UUserWidget> LobbyTemp;
	

	
	
};
