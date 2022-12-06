// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PBBuilding.generated.h"

class UPBHealthComponent;
class UWidgetComponent;
class APBPlayerState;
class APBCommand;
class APBCharacter;
class APBTurret;

UCLASS()
class PB_API APBBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APBBuilding();

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Components")
	UPBHealthComponent* HealthComponent;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHealthChanged(UPBHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDestroy();

	/*UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Components")
	UPBHealthComponent* HealthComponent;

	UFUNCTION()
	void OnHealthChanged(UPBHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);*/

	UPROPERTY(Replicated, EditDefaultsOnly, BluePrintReadOnly, Category = "UI")
	UWidgetComponent* HealthWidget;

public:	

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "PB")
	APBPlayerState* OwningPlayer;

	APBTurret* OwningTurret;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "PB")
    UStaticMeshComponent* Building;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "PB")
	bool bDied;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PB")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PB")
	float MaxShield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PB")
	float Shield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PB")
	float HealthRegenRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PB")
	float ShieldRegenRate;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "PB")
	class USphereComponent* OwnedArea;

	UPROPERTY(EditAnywhere)
	int Radius = 20000;

	UPROPERTY(EditAnywhere)
	int Depth = 100000;

	UPROPERTY(EditAnywhere)
	class UDecalComponent* AreaDecal;

	UPROPERTY(EditAnywhere)
	bool DecalVisible;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PB")
	bool bPlaceable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PB")
	APBCommand* OwningStation;

	void DecalVis(bool hide);
	
};
