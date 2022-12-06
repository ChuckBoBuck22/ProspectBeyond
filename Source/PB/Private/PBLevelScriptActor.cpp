// Fill out your copyright notice in the Description page of Project Settings.


#include "PBLevelScriptActor.h"
#include "Net/UnrealNetwork.h"

APBLevelScriptActor::APBLevelScriptActor()
{
	StationOwner = "Open";
	if (GetLevel())
	{
		StationName = GetLevel()->GetOuter()->GetFName().ToString();
	}
	
}

void APBLevelScriptActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBLevelScriptActor, StationName);
	DOREPLIFETIME(APBLevelScriptActor, StationOwner);

}