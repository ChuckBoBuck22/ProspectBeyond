// Fill out your copyright notice in the Description page of Project Settings.


#include "PBPowerUpActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APBPowerUpActor::APBPowerUpActor()
{
	PowerUpInterval = 0.f;
	TotalNumOfTicks = 0;

	bIsPowerUpActive = false;
	bReplicates = true;
}

void APBPowerUpActor::OnRep_PowerUpActive()
{
	OnPowerUpStateChange(bIsPowerUpActive);
}



void APBPowerUpActor::OnTickPowerUp()
{
	TicksProcessed++;

	OnPowerUpTicked();

	if (TotalNumOfTicks < TicksProcessed)
	{
		OnExpired();
		bIsPowerUpActive = false;
		OnRep_PowerUpActive();
		GetWorldTimerManager().ClearTimer(TimerHandlePowerUpTick);
	}
}

void APBPowerUpActor::ActivatePowerUp(AActor* ActivatedPlayer)
{
	OnActivated(ActivatedPlayer);

	bIsPowerUpActive = true;
	OnRep_PowerUpActive();
	
	if (PowerUpInterval > 0.f)
	{
		GetWorldTimerManager().SetTimer(TimerHandlePowerUpTick, this, &APBPowerUpActor::OnTickPowerUp, PowerUpInterval, true);
	}
	else
	{
		OnTickPowerUp();
	}
}

void APBPowerUpActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBPowerUpActor, bIsPowerUpActive);

}





