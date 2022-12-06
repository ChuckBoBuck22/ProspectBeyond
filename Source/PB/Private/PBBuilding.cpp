// Fill out your copyright notice in the Description page of Project Settings.


#include "PBBuilding.h"
#include "PBCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "PB/Public/Components/PBHealthComponent.h"
#include "Components/WidgetComponent.h"
#include "PBHealthBarUI.h"
#include "Kismet/GameplayStatics.h"
#include "PBCommand.h"
#include "PBPlayerState.h"
#include "PBTurret.h"
#include "Net/UnrealNetwork.h"

// Sets default values
// Sets default values
APBBuilding::APBBuilding()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// Setup components

	//Static mesh component for the actual building
	Building = CreateDefaultSubobject<UStaticMeshComponent>(FName("Building"));
	if (!ensure(Building != nullptr)) return;
	Building->SetReceivesDecals(false);
	SetRootComponent(Building);

	//Sphere component to designate area owned/influenced by the building
	OwnedArea = CreateDefaultSubobject<USphereComponent>(FName("OwnedArea"));
	if (!ensure(OwnedArea != nullptr)) return;


	//radius can be set in blueprints and controls the area owned by the building. Turret range will be controlled this way.
	OwnedArea->SetSphereRadius(Radius);
	OwnedArea->SetupAttachment(GetRootComponent());

	//The decal is to highlight the region owned by the the building
	AreaDecal = CreateDefaultSubobject<UDecalComponent>(FName("AreaDecal"));
	if (!ensure(AreaDecal != nullptr)) return;
	AreaDecal->SetupAttachment(OwnedArea);
	AreaDecal->DecalSize = FVector(Radius, Radius, Radius);
	AreaDecal->SetRelativeRotation(FRotator(0, 0, 0));
	this->DecalVis(false);

	HealthComponent = CreateDefaultSubobject<UPBHealthComponent>(TEXT("HealthComp"));
	
	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	HealthWidget->SetRelativeLocation(Building->GetSocketLocation("WidgetSocket"));
	HealthWidget->SetupAttachment(RootComponent);
	HealthWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	

	MaxHealth = 101;
	HealthRegenRate = 1;
	bDied = false;
	bReplicates = true;
	OwnedArea->SetIsReplicated(true);
	HealthComponent->DefaultHealth = MaxHealth;
	HealthComponent->HealthRegenRate = HealthRegenRate;
}


void APBBuilding::BeginPlay()
{
	Super::BeginPlay();
	
	if (!GetClass()->GetName().Contains("Ghost"))
	{
		UPBHealthBarUI* widget = Cast<UPBHealthBarUI>(HealthWidget->GetUserWidgetObject());
		if (widget)
		{
			widget->Building = this;
			widget->BuildingOwner = GetOwner();
			HealthWidget->SetWidget(widget);
			HealthWidget->SetUsingAbsoluteLocation(true);
			HealthWidget->SetWorldLocation(Building->GetSocketLocation("WidgetSocket"));
		}
		
	}

	HealthComponent->OnHealthChanged.AddDynamic(this, &APBBuilding::OnHealthChanged);
	APBPlayerState* playerState = Cast<APBPlayerState>(Owner);
	if (playerState)
	{
		HealthComponent->SetFactionNumber(StaticCast<uint8, EPBFaction>(playerState->GetFaction()));
	}
}

void APBBuilding::OnHealthChanged(UPBHealthComponent* HealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{

	if (Health <= 0 && !bDied)
	{
		bDied = true;
		ServerDestroy();
		SetLifeSpan(10);
	}
}

void APBBuilding::ServerDestroy_Implementation()
{
	if (this->IsA<APBCommand>())
	{
		APBPlayerState* playerState = Cast<APBPlayerState>(GetOwner());
		playerState->RemoveStation(Cast<APBCommand>(this));
	}
	if (OwningTurret)
	{
		OwningTurret->Destroy();
	}
	bDied = true;
	Destroy();
}

bool APBBuilding::ServerDestroy_Validate()
{
	return true;
}

void APBBuilding::DecalVis(bool hide)
{
	AreaDecal->SetVisibility(hide);
}

void APBBuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBBuilding, Building);
	DOREPLIFETIME(APBBuilding, OwnedArea);
	DOREPLIFETIME(APBBuilding, OwningPlayer);
	DOREPLIFETIME(APBBuilding, bDied);
	DOREPLIFETIME(APBBuilding, HealthWidget);
}
