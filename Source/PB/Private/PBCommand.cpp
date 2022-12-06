// Fill out your copyright notice in the Description page of Project Settings.


#include "PBCommand.h"
#include "PBPlayerController.h"
#include "PBResBldng.h"
#include "PBCharacter.h"
#include "PBDepot.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "PB/Public/Components/PBHealthComponent.h"
#include "Components/WidgetComponent.h"
#include "PBHealthBarUI.h"
#include "PBPlayerState.h"
#include "PBDIalogueUI.h"
#include "PBPlayerController.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Net/UnrealNetwork.h"
#include "PBLevelScriptActor.h"

APBCommand::APBCommand()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<UUserWidget> stationDialogue(TEXT("WidgetBlueprint'/Game/UI/W_StationNameDialogue'"));
	DialogueTemp = stationDialogue.Class;

	// Setup components

	WallOne = CreateDefaultSubobject<UBoxComponent>(FName("WallOne"));
	if (!ensure(WallOne != nullptr)) return;
	WallOne->SetupAttachment(GetRootComponent());
	WallOne->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	WallOne->SetBoxExtent(FVector(Radius, 10, Depth));
	WallOne->SetRelativeLocation(FVector(0, Radius, Radius - Depth));


	WallTwo = CreateDefaultSubobject<UBoxComponent>(FName("WallTwo"));
	if (!ensure(WallTwo != nullptr)) return;
	WallTwo->SetupAttachment(GetRootComponent());
	WallTwo->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	WallTwo->SetBoxExtent(FVector(Radius, 10, Depth));
	WallTwo->SetRelativeLocation(FVector(0, -Radius, Radius - Depth));

	WallThree = CreateDefaultSubobject<UBoxComponent>(FName("WallThree"));
	if (!ensure(WallThree != nullptr)) return;
	WallThree->SetupAttachment(GetRootComponent());
	WallThree->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	WallThree->SetBoxExtent(FVector(10, Radius, Depth));
	WallThree->SetRelativeLocation(FVector(Radius, 0, Radius - Depth));

	WallFour = CreateDefaultSubobject<UBoxComponent>(FName("WallFour"));
	if (!ensure(WallFour != nullptr)) return;
	WallFour->SetupAttachment(GetRootComponent());
	WallFour->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	WallFour->SetBoxExtent(FVector(10, Radius, Depth));
	WallFour->SetRelativeLocation(FVector(-Radius, 0, Radius - Depth));

	WallFive = CreateDefaultSubobject<UBoxComponent>(FName("WallFive"));
	if (!ensure(WallFive != nullptr)) return;
	WallFive->SetupAttachment(GetRootComponent());
	WallFive->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	WallFive->SetBoxExtent(FVector(Radius, Radius, 10));
	WallFive->SetRelativeLocation(FVector(0, 0, Radius));

	//Add OverlapActor
	TriggerArea = CreateDefaultSubobject<UBoxComponent>(FName("TriggerArea"));
	if (!ensure(TriggerArea != nullptr)) return;
	TriggerArea->SetupAttachment(Building);
	TriggerArea->SetBoxExtent(FVector(230, 220, 110));
	MaxHealth = 1000.f;
	Buildings.Empty();
	bHasDepot = false;
}

void APBCommand::BeginPlay()
{
	Super::BeginPlay();
	AddDialogueUI();
	TriggerArea->OnComponentBeginOverlap.AddDynamic(this, &APBCommand::TriggerAreaBeginOverlap);
	TriggerArea->OnComponentEndOverlap.AddDynamic(this, &APBCommand::TriggerAreaEndOverlap);
	OwnedArea->OnComponentBeginOverlap.AddDynamic(this, &APBCommand::OwnedAreaBeginOverlap);
	OwnedArea->OnComponentEndOverlap.AddDynamic(this, &APBCommand::OwnedAreaEndOverlap);
	FTimerHandle timerHandle;
	GetWorldTimerManager().SetTimer(timerHandle, this, &APBCommand::Update, 2, true);
	//UpdateAreaNamesOnNew();
	
	
}


void APBCommand::AddDialogueUI()
{
	APBPlayerState* playerState = Cast<APBPlayerState>(GetOwner());
	playerState->AddStation(this);
	APBPlayerController* pc = Cast<APBPlayerController>(playerState->GetOwner());
	if (pc)
	{
		pc->AddDialogueUI(this);
	}
	
	
}

void APBCommand::Update()
{
	float a = 0, b = 0, c = 0;
	TArray<AActor*> result;
	GetOverlappingActors(result, APBBuilding::StaticClass());
	if (!result.IsEmpty())
	{
		for (int i = 0; i < result.Num(); i++)
		{
			if (!result[i]->GetClass()->GetAuthoredName().Contains("Ghost"))
			{
				APBBuilding* building = Cast<APBBuilding>(result[i]);
				if (!Buildings.Contains(building) && building->GetOwner() == this->GetOwner())
				{
					Buildings.Add(building);
				}
				if (building->IsA<APBResBldng>() && building->OwningStation == this)
				{
					APBResBldng* resource = Cast<APBResBldng>(building);
					a += resource->ResourceAProduction;
					b += resource->ResourceBProduction;
					c += resource->ResourceCProduction;
				}
			}
			
		}
		ResourceA_Production = a;
		ResourceB_Production = b;
		ResourceC_Production = c;
		ResourceA_Stores += ResourceA_Production;
		ResourceB_Stores += ResourceB_Production;
		ResourceC_Stores += ResourceC_Production;
	}
}

void APBCommand::SetStationName(FString _StationName)
{
	APBLevelScriptActor* LevelScript = Cast<APBLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	if (LevelScript)
	{
		StationName = _StationName;
		LevelScript->StationName = StationName;
	}
}



void APBCommand::TriggerAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APBCharacter::StaticClass()) && Cast<APBCharacter>(OtherActor)->GetPlayerState() == this->Owner)
	{
		this->DecalVis(true);
		Cast<APBPlayerController>(Cast<APBCharacter>(OtherActor)->GetController())->EnterBuildMode(this);
	}
}


void APBCommand::TriggerAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void APBCommand::OwnedAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APBBuilding::StaticClass()) && OtherActor->GetOwner() == this->Owner && !OtherActor->GetName().Contains("Ghost"))
	{
		APBBuilding* building = Cast<APBBuilding>(OtherActor);
		building->OwningStation = this;
		AddBuilding(building);

	}
	/*if (OtherActor->IsA(APBCharacter::StaticClass()))
	{
		APlayerState* playerState = Cast<APBCharacter>(OtherActor)->GetPlayerState();
		Cast<APBPlayerState>(playerState)->SetAreaName(StationName);
	}*/

}

void APBCommand::OwnedAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(APBBuilding::StaticClass()) && OtherActor->GetOwner() == this->Owner)
	{
		RemoveBuilding(Cast<APBBuilding>(OtherActor));
	}
	/*if (OtherActor->IsA(APBCharacter::StaticClass()))
	{
		APlayerState* playerState = Cast<APBCharacter>(OtherActor)->GetPlayerState();
		Cast<APBPlayerState>(playerState)->SetAreaLevelName();
	}*/
}

void APBCommand::AddBuilding(APBBuilding* buildingToAdd)
{
	if (!Buildings.Contains(buildingToAdd))
	{
		Buildings.Add(buildingToAdd);
		if (buildingToAdd->IsA<APBDepot>())
		{
			bHasDepot = true;
		}
	}
	
}

void APBCommand::RemoveBuilding(APBBuilding* buildingToRemove)
{
	Buildings.Remove(buildingToRemove);
}

//void APBCommand::UpdateAreaNamesOnNew()
//{
//	if (IsActorInitialized() && !StationName.IsEmpty())
//	{
//		TArray<AActor*> result;
//		GetOverlappingActors(result, APBCharacter::StaticClass());
//		if (!result.IsEmpty())
//		{
//			for (int i = 0; i < result.Num(); i++)
//			{
//				APBCharacter* playerChar = Cast<APBCharacter>(result[i]);
//				Cast<APBPlayerState>(playerChar->GetPlayerState())->SetAreaName(StationName);
//			}
//		}
//		return;
//	}
//	FTimerHandle timerHandle;
//	GetWorldTimerManager().SetTimer(timerHandle, this, &APBCommand::UpdateAreaNamesOnNew, 5, false, 5);
//	
//}

void APBCommand::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBCommand, StationName);

}


