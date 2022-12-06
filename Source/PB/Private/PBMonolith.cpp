// Fill out your copyright notice in the Description page of Project Settings.


#include "PBMonolith.h"
#include "PBCommand.h"

// Sets default values
APBMonolith::APBMonolith()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComp"));
	SetRootComponent(MeshComp);

	Stations = TArray<AActor*>();
	
}

// Called when the game starts or when spawned
void APBMonolith::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(DesignationHandle, this, &APBMonolith::Designate, 60, true, 120);
}

// Called every frame
void APBMonolith::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

APBCommand* APBMonolith::DesignateStation()
{
	Stations = GetLevel()->Actors.FilterByPredicate([](AActor* Actor) {if (Actor) { return Actor->IsA(APBCommand::StaticClass()); } return false; });
	if (!Stations.IsEmpty())
	{
		if (DesignatedStation)
		{
			DesignatedStation->bIsDesignated = false;
			Stations.Remove((AActor*)DesignatedStation);
			DesignatedStation = nullptr;
		}
		if (!Stations.IsEmpty())
		{
			DesignatedStation = (APBCommand*)Stations[FMath::RandHelper(Stations.Num() - 1)];
			DesignatedStation->bIsDesignated = true;
			return DesignatedStation;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Stations List Empty! Only station in list was prior designated"));
			return nullptr;
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Stations List Empty!"));
		return nullptr;
	}
}

void APBMonolith::Designate()
{
	DesignateStation();
}
	

