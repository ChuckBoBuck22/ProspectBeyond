// Fill out your copyright notice in the Description page of Project Settings.


#include "PBResBldng.h"
#include "PBCommand.h"
#include "PBGameStateBase.h"
#include "Engine/DecalActor.h"
#include "Engine/Texture2d.h"
#include "Components/DecalComponent.h"
#include "Net/UnrealNetwork.h"

APBResBldng::APBResBldng()
{
	MaxHealth = 300;
	HealthRegenRate = 1;

	
	
}

void APBResBldng::BeginPlay()
{
	Super::BeginPlay();
	
}

void APBResBldng::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBResBldng, ResourceAProduction);
	DOREPLIFETIME(APBResBldng, ResourceBProduction);
	DOREPLIFETIME(APBResBldng, ResourceCProduction);

}
