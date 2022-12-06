// Fill out your copyright notice in the Description page of Project Settings.


#include "PBDepot.h"

// Sets default values
APBDepot::APBDepot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	DepotLevel = 0;
}

// Called when the game starts or when spawned
void APBDepot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APBDepot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

