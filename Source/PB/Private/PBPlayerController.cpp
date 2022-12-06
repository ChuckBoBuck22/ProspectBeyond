// Fill out your copyright notice in the Description page of Project Settings.


#include "PBPlayerController.h"
#include "PBBuildPawn.h"
#include "PB/Public/PBCharacter.h"
#include "PBPlayerState.h"
#include "PBCommand.h"
#include "PBTurret.h"
#include "PBDialogueUI.h"
#include "PBHudUI.h"
#include "PB/Public/Components/PBHealthComponent.h"
#include "PBMonolith.h"
#include "PBWeapon.h"
#include "Components/DecalComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DefaultPawn.h"
#include "Engine/World.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerInput.h" 
#include "Components/InputComponent.h"
#include "Engine/LevelScriptBlueprint.h"
#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"
#include "Engine/LevelScriptActor.h"
#include "Misc/OutputDeviceNull.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "LandscapeStreamingProxy.h"
#include "PBGameStateBase.h"
#include "Engine/DecalActor.h"
#include "PBStatics.h"
#include "PBResBldng.h"
#include "PBLobbyUI.h"
#include "Networking/PBOnlineBeaconClient.h"
#include "Kismet/KismetStringLibrary.h"
#include "Engine/NetworkDelegates.h"
#include "PBLevelScriptActor.h"


APBPlayerController::APBPlayerController()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> stationDialogue(TEXT("WidgetBlueprint'/Game/UI/W_StationNameDialogue'"));
	DialogueTemp = stationDialogue.Class;
	static ConstructorHelpers::FClassFinder<APBBuildPawn> buildPawn(TEXT("Blueprint'/Game/Blueprints/Character/B_PBBuildPawn'"));
	BuildPawnTemp = buildPawn.Class;
	static ConstructorHelpers::FClassFinder<UUserWidget> hud(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/W_PBHUD'"));
	HudTemp = hud.Class;
	static ConstructorHelpers::FClassFinder<APBCharacter> blueprintCharacter(TEXT("Blueprint'/Game/Blueprints/Character/B_PBCharacter'"));
	CharacterTemp = blueprintCharacter.Class;
	static ConstructorHelpers::FClassFinder<APBBuilding> blueprintCommandGhost(TEXT("Blueprint'/Game/Blueprints/Buildings/B_PBResBldng'"));
	CommandTemp = blueprintCommandGhost.Class;
	static ConstructorHelpers::FClassFinder<UUserWidget> lobbymenu(TEXT("WidgetBlueprint'/Game/UI/W_LobbyMenu'"));
	LobbyTemp = lobbymenu.Class;
	bReplicates = true;
	 
}


void APBPlayerController::BeginPlay()
{
	Super::BeginPlay();
	//APlayerState* playerState = GetPlayerState<APlayerState>();
	if (GetLocalRole() < ROLE_Authority)
	{
		if (HudTemp && !GetWorld()->GetLevel(0)->GetOuter()->GetName().Contains("Lobby"))
		{
			HudUI = CreateWidget<UPBHudUI>(this, HudTemp);
			if (HudUI)
			{
				HudUI->LevelScript = Cast<APBLevelScriptActor>(GetWorld()->GetLevel(0)->LevelScriptActor.Get());
				HudUI->SetOwningPlayer(this);
				SetHudUIPlayerState();
				HudUI->SetVisibility(ESlateVisibility::Visible);
				HudUI->AddToViewport();
			}
		}

		if (GetWorld()->GetLevel(0)->GetOuter()->GetName().Contains("Lobby"))
		{
			LobbyUI = CreateWidget<UPBLobbyUI>(this, LobbyTemp);
			if (LobbyUI)
			{
				LobbyUI->SetVisibility(ESlateVisibility::Visible);
				LobbyUI->AddToViewport();
			}
			
		}


		//if (playerState)
		//{
			//Cast<APBPlayerState>(playerState)->SetAreaName((FName)this->GetLevel()->GetOuter()->GetName());
			//playerState->SetAreaName((FName)this->GetLevel()->GetOuter()->GetName());
		//}
	}

}

void APBPlayerController::SetHudUIPlayerState()
{
	if (this->GetLocalRole())
	{
		if (GetLocalRole() < ROLE_Authority)
		{
			APBPlayerState* playerState = Cast<APBPlayerState>(PlayerState);
			if (playerState)
			{
				HudUI->PlayerStateHUDUI = playerState;
				return;
			}
			FTimerHandle timerHandle;
			GetWorldTimerManager().SetTimer(timerHandle, this, &APBPlayerController::SetHudUIPlayerState, 5, false, 5);
		}
	}
}

void APBPlayerController::AddWidgetToViewport(UUserWidget* WidgetToAdd)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		WidgetToAdd->SetVisibility(ESlateVisibility::Visible);
		WidgetToAdd->AddToViewport();
	}
	
}

void APBPlayerController::SetupHUD()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		
		if (LobbyUI)
		{
			LobbyUI->RemoveFromParent();
		}
		if (HudUI)
		{
			HudUI->SetOwningPlayer(this);
			SetHudUIPlayerState();
			HudUI->SetVisibility(ESlateVisibility::Visible);
			HudUI->AddToViewport();
		}
		FInputModeGameAndUI InputMode;
		SetInputMode(InputMode);
	}
	
}

void APBPlayerController::PostSeamlessTravel()
{
	APlayerController::PostSeamlessTravel();
	SetupHUD();
}

void APBPlayerController::ToggleResourceDecal()
{
	FOutputDeviceNull ar;

	GetWorld()->GetLevelScriptActor()->CallFunctionByNameWithArguments(TEXT("ToggleResourceDecal"), ar, NULL, true);
}

void APBPlayerController::EnterBuildMode(APBCommand* command)
{

	UE_LOG(LogTemp, Warning, TEXT("EnterBuildModeCalled"));
	bInBuildMode = true;
	APBCharacter* Char = Cast<APBCharacter>(this->GetCharacter());
	if (GetLocalRole() == ROLE_Authority)
	{
		bInBuildMode = true;
		if (Char)
		{
			FVector charLocation = Char->GetActorLocation();
			FRotator charRotation = Char->GetActorRotation();
			FActorSpawnParameters sp = FActorSpawnParameters();
			BuildPawn = GetWorld()->SpawnActor<APBBuildPawn>(BuildPawnTemp, sp);
			BuildPawn->SetPlayerState(Char->GetPlayerState());
			BuildPawn->SetActorLocationAndRotation(charLocation + FVector(0.0f, 0.0f, 1000), charRotation);
			BuildPawn->CurrentCommand = command;

			this->UnPossess();
			APBPlayerState* playerState = Cast<APBPlayerState>(PlayerState);
			playerState->LastCharacterLocation = charLocation;
			playerState->LastOwnedCommandVisited = command;
			Char->CurrentWeapon->Destroy();
			Char->Destroy();
			this->Possess(BuildPawn);
		}
	}

	
}

void APBPlayerController::PlaceBuilding()
{
	bPlacingBuilding = true;
	if (GhostBuilding != nullptr)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = PlayerState;
		GhostActor = GetWorld()->SpawnActor<AActor>(GhostBuilding.Get(), this->GetPawn()->GetActorLocation() + FVector(0, 500, 0), FRotator(0, 0, 0), spawnParams);
		bEnableClickEvents = true;
		if (GhostActor->IsValidLowLevel())
		{	
			GetWorldTimerManager().SetTimer(TimerHandle, this, &APBPlayerController::UpdateLocation, .1, true, .1);	
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GhostActor Not Valid"));
			bPlacingBuilding = false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GhostBuilding is null"));
		bPlacingBuilding = false;
	}
	
}

void APBPlayerController::ServerPlaceBuilding_Implementation(TSubclassOf<AActor> building, FVector loc, FRotator rot)
{
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = PlayerState;
	ActualActor = GetWorld()->SpawnActor<AActor>(building.Get(), loc, rot, spawnParams);
	ActualActor->SetOwner(PlayerState);
	if (ActualActor->IsA<APBTurret>())
	{
		Cast<APBTurret>(ActualActor)->Building->HealthComponent->SetFactionNumber((StaticCast<uint8, EPBFaction>(Cast<APBPlayerState>(PlayerState)->GetFaction())));
		Cast<APBTurret>(ActualActor)->Building->SetOwner(PlayerState);
	}
	if (ActualActor->IsA<APBResBldng>())
	{
		UTexture2D* Tex2D = Cast<UTexture2D>(Cast<APBGameStateBase>(GetWorld()->GetGameState())->ResourceDecal->GetDecalMaterial()->GetReferencedTextures()[0]);
		double X = (loc.X + 100800) / 100;
		double Y = (loc.Y + 100800) / 100;
		FLinearColor Color;
		UPBStatics::GetPixelFromT2D(Tex2D, X, Y, Color);
		DrawDebugString(GetWorld(), Location, Color.ToString(), 0, FColor::White, 1.0f);
		Cast<APBResBldng>(ActualActor)->ResourceAProduction = Color.B * 100;
		Cast<APBResBldng>(ActualActor)->ResourceBProduction = Color.G * 100;
		Cast<APBResBldng>(ActualActor)->ResourceCProduction = Color.R * 100;
	}
	
}

bool APBPlayerController::ServerPlaceBuilding_Validate(TSubclassOf<AActor> building, FVector loc, FRotator rot)
{
	return true;
}

void APBPlayerController::AddDialogueUI(AActor* actor)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		if (DialogueTemp != nullptr && actor->IsA<APBCommand>())
		{
			DialogueUI = CreateWidget<UPBDIalogueUI>(this, DialogueTemp);
			DialogueUI->SetVisibility(ESlateVisibility::Visible);
			DialogueUI->command = Cast<APBCommand>(actor);
			DialogueUI->AddToViewport();
		}
	}
	
}

void APBPlayerController::ToggleMenuMode()
{
	HudUI->ToggleMenuMode();
}

void APBPlayerController::ServerExitBuildMode_Implementation()
{
	
	if (bInBuildMode)
	{
		ClientExitBuildMode();
		APBPlayerState* playerState = Cast<APBPlayerState>(PlayerState);
		APBCharacter* newCharacter = GetWorld()->SpawnActor<APBCharacter>(CharacterTemp,
			playerState->LastOwnedCommandVisited->GetActorLocation() + (playerState->LastOwnedCommandVisited->GetActorRotation().Vector() + 2000),
			playerState->LastOwnedCommandVisited->GetActorRotation());
		newCharacter->SetPlayerState(PlayerState);
		playerState->SetAreaName(playerState->LastOwnedCommandVisited->StationName);
		playerState->LastOwnedCommandVisited->DecalVis(false);
		this->UnPossess();

		if (BuildPawn)
		{
			BuildPawn->Destroy();
		}
		this->Possess(newCharacter);
		bInBuildMode = false;
	}
}

bool APBPlayerController::ServerExitBuildMode_Validate()
{
	return true;
}

void APBPlayerController::ClientExitBuildMode_Implementation()
{
	APBPlayerState* playerState = Cast<APBPlayerState>(PlayerState);
	playerState->LastOwnedCommandVisited->DecalVis(false);
}

void APBPlayerController::SetupInputComponent()
{
	// Always call this.
	Super::SetupInputComponent();

	/*InputComponent->BindAction<APBPlayerController>("PlaceBuilding", IE_Pressed, this, &APBPlayerController::isMousePressed);
	InputComponent->BindAction<APBPlayerController>("!PlaceBuilding", IE_Pressed, this, &APBPlayerController::isRightMousePressed);
	InputComponent->BindAction<APBPlayerController>("Exit", IE_Pressed, this, &APBPlayerController::ExitBuildMode);*/
}

void APBPlayerController::Travel(FString Name)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		FString URLString = "127.0.0.1:" + Name.Right(4) + "/Game/Maps/Badlands/" + Name.Left(10);
		FURL URL;
		URL.Host = "127.0.0.1";
		URL.Port = UKismetStringLibrary::Conv_StringToInt(Name.Right(4)) + 100;
		URL.Map = "/Game/Maps/Badlands/" + Name.Left(10);
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		APBOnlineBeaconClient* BeaconClient = GetWorld()->SpawnActor<APBOnlineBeaconClient>(APBOnlineBeaconClient::StaticClass(), SpawnParams);
		BeaconClient->SetOwner(this);
		
		if (BeaconClient->ConnectToServer(Name))
		{
			
			if (BeaconClient->GetNetConnection()->State == EConnectionState::USOCK_Open)
			{
				BeaconClient->DestroyBeacon();
				ClientTravel(URLString, ETravelType::TRAVEL_Absolute, false);
			}
		}
	}
}

void APBPlayerController::TravelHelper(APBOnlineBeaconClient* BeaconClient, FString URLString, FURL URL, FString Name, int count, bool ServerCreated)
{
	if (count > 10000)
	{
		return;
	}
	else if (BeaconClient->GetConnectionState() == EBeaconConnectionState::Open)
	{
		BeaconClient->DestroyBeacon();
		ExecuteTravel(URLString);
		return;
	}
	/*else if (BeaconClient->GetConnectionState() == EBeaconConnectionState::Invalid && !ServerCreated)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server not found, attempting to create"));
		FString CommandString = "C:/Repos/PB/Packaged/WindowsServer/PBServer.exe";
		FString Params = "/Game/Maps/Badlands/" + Name.Left(10) + " -port=" + Name.Right(4) + " -log" + " -nosteam";
		FPlatformProcess::CreateProc(*CommandString, *Params, true, false, false, nullptr, 0, nullptr, nullptr);
		ServerCreated = true;
	}*/
	else
	{
		int newCount = count + 1;
		TravelHelper(BeaconClient, URLString, URL, Name, newCount, ServerCreated);
	}
}

void APBPlayerController::ExecuteTravel(FString Destination)
{
	ClientTravel("127.0.0.1:" + Destination.Right(4) + "/Game/Maps/Badlands/" + Destination.Left(10), ETravelType::TRAVEL_Absolute, false);
}

void APBPlayerController::ServerCreateServer_Implementation(const FString& DestinationAddress)
{
	UE_LOG(LogTemp, Warning, TEXT("Server not found, attempting to create"));
	FString CommandString = "C:/Repos/PB/Packaged/WindowsServer/PBServer.exe";
	FString Params = "/Game/Maps/Badlands/" + DestinationAddress.Left(10) + " -port=" + DestinationAddress.Right(4) + " -log" + " -nosteam";
	FPlatformProcess::CreateProc(*CommandString, *Params, true, false, false, nullptr, 0, nullptr, nullptr);
	
}

bool APBPlayerController::ServerCreateServer_Validate(const FString& DestinationAddress)
{
	return true;
}

void APBPlayerController::isMousePressed()
{
	if (bPlacingBuilding && Cast<APBBuilding, AActor>(GhostActor)->bPlaceable)
	{
		bPlaced = true;
	}
}

void APBPlayerController::isRightMousePressed()
{
	bPlacingBuilding = false;
}

void APBPlayerController::UpdateLocation()
{
	if (!bPlacingBuilding)
	{
		if (GhostActor->IsValidLowLevelFast())
		{
			GhostActor->Destroy();
		}
		bPlaced = false;
	}
	if (bPlaced)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle);
		bPlacingBuilding = false;
		//ActualActor = GetWorld()->SpawnActor<AActor>(BuildingToPlace.Get(), GhostActor->GetActorLocation(), FRotator(0, 0, 0));
		ServerPlaceBuilding(BuildingToPlace, GhostActor->GetActorLocation(), FRotator(0.f));
		GhostActor->Destroy(true, true);
		GhostBuilding = nullptr;
		bPlaced = false;
		Location = FVector(0, 0, 0);
		BuildingToPlace = nullptr;
		
		
	}
	else if (bPlacingBuilding && GhostActor != nullptr)
	{
		FHitResult hit;
		this->GetHitResultUnderCursor(ECC_GameTraceChannel2, true, hit);
		Location = hit.Location;
		GhostActor->SetActorLocation(Location);
		APawn* controlledPawn = this->GetPawn();
		FString name = GhostActor->GetClass()->GetName();
		if (controlledPawn != nullptr)
		{
			TArray<AActor*> overlappedActors;
			GetPawn()->GetOverlappingActors(overlappedActors, APBCommand::StaticClass());
			
			if (!overlappedActors.IsEmpty())
			{
				LocalCommand = overlappedActors[0];
			}
			if (name.Contains("CommandGhost"))
			{
				TArray<AActor*> actors;
				TArray<AActor*> monoliths;
				GhostActor->GetOverlappingActors(actors, APBCommand::StaticClass());
				GhostActor->GetOverlappingActors(monoliths, APBMonolith::StaticClass());
				if (actors.IsEmpty() && bInBuildMode == false && monoliths.IsEmpty())
				{
					Cast<APBBuilding, AActor>(GhostActor)->bPlaceable = true;
					
				}
				else
				{
					Cast<APBBuilding, AActor>(GhostActor)->bPlaceable = false;
					
				}
			}
			else if (name.Contains("DepotGhost"))
			{
				TArray<AActor*> actors;
				GhostActor->GetOverlappingActors(actors, APBCommand::StaticClass());

				if (!actors.IsEmpty() && actors[0] == LocalCommand && LocalCommand->GetOwner() == PlayerState && Cast<APBCommand>(actors[0])->bIsDesignated && !Cast<APBCommand>(LocalCommand)->bHasDepot)
				{
					Cast<APBBuilding, AActor>(GhostActor)->bPlaceable = true;
				}
				else
				{
					Cast<APBBuilding, AActor>(GhostActor)->bPlaceable = false;
				}
			}
			else if (name.Contains("Res"))
			{	
				if (GetLocalRole() < ROLE_Authority)
				{
					UTexture2D* Tex2D = Cast<UTexture2D>(Cast<APBGameStateBase>(GetWorld()->GetGameState())->ResourceDecal->GetDecalMaterial()->GetReferencedTextures()[0]);
					double X = (hit.Location.X + 100800) / 100;
					double Y = (hit.Location.Y + 100800) / 100;
					FLinearColor Color;
					UPBStatics::GetPixelFromT2D(Tex2D, X, Y, Color);
					DrawDebugString(GetWorld(), Location, Color.ToString(), 0, FColor::White, 1.0f);
					Cast<APBResBldng>(GhostActor)->ResourceAProduction = Color.B * 100;
					Cast<APBResBldng>(GhostActor)->ResourceBProduction = Color.G * 100;
					Cast<APBResBldng>(GhostActor)->ResourceCProduction = Color.R * 100;
				}
				
				TArray<AActor*> actors;
				GhostActor->GetOverlappingActors(actors, APBCommand::StaticClass());

				if (!actors.IsEmpty() && actors[0] == LocalCommand && LocalCommand->GetOwner() == PlayerState)
				{
					Cast<APBBuilding, AActor>(GhostActor)->bPlaceable = true;
				}
				else
				{
					Cast<APBBuilding, AActor>(GhostActor)->bPlaceable = false;
				}
			}
			else
			{
				TArray<AActor*> actors;
				GhostActor->GetOverlappingActors(actors, APBCommand::StaticClass());
				
				if (!actors.IsEmpty() && actors[0] == LocalCommand && LocalCommand->GetOwner() == PlayerState)
				{
					Cast<APBBuilding, AActor>(GhostActor)->bPlaceable = true;
				}
				else
				{
					Cast<APBBuilding, AActor>(GhostActor)->bPlaceable = false;
				}
			}
		}
		LocalCommand = nullptr;
		
	}
}

void APBPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBPlayerController, ActualActor);
	DOREPLIFETIME(APBPlayerController, bInBuildMode);

}




