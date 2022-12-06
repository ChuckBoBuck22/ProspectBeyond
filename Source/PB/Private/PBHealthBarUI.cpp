// Fill out your copyright notice in the Description page of Project Settings.


#include "PBHealthBarUI.h"
#include "Net/UnrealNetwork.h"

void UPBHealthBarUI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPBHealthBarUI, BuildingOwner);
	
}
