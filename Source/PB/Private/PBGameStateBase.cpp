// Fill out your copyright notice in the Description page of Project Settings.


#include "PBGameStateBase.h"
#include "PBPlayerState.h"
#include "Net/UnrealNetwork.h"

void APBGameStateBase::OnRep_wildphireScore()
{

}

void APBGameStateBase::OnRep_AlphaAspectScore()
{
}

void APBGameStateBase::OnRep_procloutScore()
{
}

void APBGameStateBase::OnRep_SenseDynamicScore()
{
}

void APBGameStateBase::OnRep_SportingPowerScore()
{
}

void APBGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBGameStateBase, wildphireScore);
	DOREPLIFETIME(APBGameStateBase, AlphaAspectScore);
	DOREPLIFETIME(APBGameStateBase, procloutScore);
	DOREPLIFETIME(APBGameStateBase, SenseDynamicScore);
	DOREPLIFETIME(APBGameStateBase, SportingPowerScore);
}
