// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PBHealthComponent.h"
#include "PBBuilding.h"
#include "PBPlayerState.h"
#include "PBTurret.h"
#include "PBWeapon.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UPBHealthComponent::UPBHealthComponent()
{
	
	SetIsReplicatedByDefault(true);
	
	Health = DefaultHealth;
	/* Default Team Assignment */
	FactionNumber = 255;
}


bool UPBHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)
	{
		return false;
	}
	UPBHealthComponent* HealthCompA = Cast<UPBHealthComponent>(ActorA->GetComponentByClass(UPBHealthComponent::StaticClass()));
	UPBHealthComponent* HealthCompB = Cast<UPBHealthComponent>(ActorB->GetComponentByClass(UPBHealthComponent::StaticClass()));

	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		return false;
	}
	return HealthCompA->GetFactionNumber() == HealthCompB->GetFactionNumber();
}

// Called when the game starts
void UPBHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = DefaultHealth;
	if (GetOwnerRole() == ROLE_Authority)
	{
		Health = DefaultHealth;
		
		MyOwner = GetOwner();
		
		
		if (MyOwner)
		{
			if (MyOwner->IsA<APBBuilding>())
			{
				APBPlayerState* playerState = Cast<APBPlayerState>(MyOwner->GetOwner());
				if (playerState)
				{
					FactionNumber = StaticCast<uint8, EPBFaction>(playerState->GetFaction());
				}
			}
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UPBHealthComponent::HandleTakeAnyDamage);
		}
	}

	
	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &UPBHealthComponent::Update, 2, true, 10);
}

void UPBHealthComponent::Update()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (HealthRegenRate && (Health < DefaultHealth) && Health > 0)
		{
			//Health = FMath::Clamp(Health + HealthRegenRate, 0.0f, MaxHealth);
			//HandleTakeAnyDamage(GetOwner(), -HealthRegenRate, nullptr, nullptr, nullptr);
			Health = FMath::Clamp(Health + HealthRegenRate, 0.0f, DefaultHealth);
			OnHealthChanged.Broadcast(this, Health, -HealthRegenRate, nullptr, nullptr, nullptr);
		}

	}
	
}

/// <summary>
/// Applies Damage to the Health Variable
/// </summary>
/// <param name="DamagedActor"></param>
/// <param name="Damage"></param>
/// <param name="DamageType"></param>
/// <param name="InstigatedBy"></param>
/// <param name="DamageCauser"></param>
void UPBHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
  {
	/*if (Damage <= 0.0f)
	{
		return;
	}*/

	if (DamageCauser != DamagedActor && IsFriendly(DamagedActor, DamageCauser))
	{
		return;
	}
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
	
}

void UPBHealthComponent::OnRepHealth(float OldHealth)
{
	float Damage = Health - OldHealth;
	OnHealthChanged.Broadcast(this, Health, Damage , nullptr, nullptr, nullptr);
}

void UPBHealthComponent::OnRepFaction(uint8 FactionNum)
{
	OnFactionChanged.Broadcast(this, FactionNum);
}

void UPBHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || Health <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health + HealAmount, 0.0f, DefaultHealth);
	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(HealAmount));
	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}


void UPBHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPBHealthComponent, Health);
	DOREPLIFETIME(UPBHealthComponent, FactionNumber);

}

