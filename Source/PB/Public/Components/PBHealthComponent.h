// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PBHealthComponent.generated.h"


class UDamageType;
class AController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams( FOnHealthChangedSignature, UPBHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FOnFactionChangedSignature, UPBHealthComponent*, HealthComp, uint8, NewFactionNumber);


UCLASS( ClassGroup=(PB), meta=(BlueprintSpawnableComponent) )
class PB_API UPBHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPBHealthComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	AActor* MyOwner;

	UPROPERTY(ReplicatedUsing=OnRepFaction, BlueprintReadOnly, Category = "HealthComponent")
	uint8 FactionNumber;

	UFUNCTION(BlueprintCallable, Category = "Teams")
	FORCEINLINE uint8 GetFactionNumber() const { return FactionNumber; }
	FORCEINLINE void SetFactionNumber(uint8 FactionToSet) { FactionNumber = FactionToSet; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HealthComponent")
	static bool IsFriendly(AActor* ActorA, AActor* ActorB);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	float DefaultHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
	float HealthRegenRate;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void Update();
	
	UPROPERTY(ReplicatedUsing=OnRepHealth, BlueprintReadOnly, Category = "HealthComponent")
	float Health;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnRepHealth(float OldHealth);

	UFUNCTION()
	void OnRepFaction(uint8 FactionNum);

public:
	UFUNCTION(BlueprintCallable, Category = "HealthComponent")
	void Heal(float HealAmount);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnFactionChangedSignature OnFactionChanged;


	

		
};
