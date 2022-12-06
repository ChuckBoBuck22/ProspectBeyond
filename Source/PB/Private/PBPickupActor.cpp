// Fill out your copyright notice in the Description page of Project Settings.


#include "PBPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "PBPowerUpActor.h"
#include "TimerManager.h"
#include "PBCharacter.h"

// Sets default values
APBPickupActor::APBPickupActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75);
	RootComponent = SphereComp;

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetRelativeRotation(FRotator(90, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64, 75, 75);
	DecalComp->SetupAttachment(RootComponent);

	CoolDownDuration = 10.0f;
	bReplicates = true;
}

// Called when the game starts or when spawned
void APBPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetLocalRole() == ROLE_Authority)
	{
		Respawn();
	}
}

void APBPickupActor::Respawn()
{
	if (PowerUpClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PowerUpClass is nullptr in %s. Please update blueprint"), *GetName());
		return;
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PowerUpInstance = GetWorld()->SpawnActor<APBPowerUpActor>(PowerUpClass, GetTransform());
}

void APBPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (GetLocalRole() == ROLE_Authority && PowerUpInstance && OtherActor->IsA<APBCharacter>())
	{
		PowerUpInstance->ActivatePowerUp(OtherActor);
		PowerUpInstance = nullptr;

		GetWorldTimerManager().SetTimer(RespawnTimer, this, &APBPickupActor::Respawn, CoolDownDuration);
	}
}



