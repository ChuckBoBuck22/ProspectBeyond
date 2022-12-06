// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PBFaction.h"
#include "PBCharacter.generated.h"

class APBWeapon;
class UPBHealthComponent;
class UInputMappingContext;
class UInputAction;
class UMaterialInstance;

UCLASS(config=Game)
class APBCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	APBCharacter();
	virtual void PawnClientRestart() override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void BeginCrouch();
	void EndCrouch();

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void PlaceBuildingInputAction();
	void PlaceBuildingEscapeInputAction();
	// End of APawn interface

	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV;

	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed;

	void BeginZoom();
	void EndZoom();
	void ToggleMenuMode();

	UPROPERTY(EditAnywhere, Category = "Player")
	UMaterialInstance* wildphireMatInst;

	UPROPERTY(EditAnywhere, Category = "Player")
	UMaterialInstance* AlphaAspectMatInst;

	UPROPERTY(EditAnywhere, Category = "Player")
	UMaterialInstance* procloutMatInst;

	UPROPERTY(EditAnywhere, Category = "Player")
	UMaterialInstance* SenseDynamicMatInst;

	UPROPERTY(EditAnywhere, Category = "Player")
	UMaterialInstance* SportingPowerMatInst;

	UPROPERTY(EditAnywhere, Category = "Player")
	UMaterialInstance* ProtectorateMatInst;

	UPROPERTY(EditAnywhere, Category = "Player")
	UMaterialInstance* NoFactionMatInst;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<APBWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	UFUNCTION()
	void OnHealthChanged(UPBHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDeath();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetFaction(uint8 FactionNum);

	UPROPERTY(Replicated, BluePrintReadOnly, Category = "Player")
	bool bDied;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* InputMappingContextGameplay;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* InputMappingContextMenuMode;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ZoomAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* CrouchAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* FireAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MenuModeAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* PlaceBuildingAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* PlaceBuildingEscapeAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ToggleResourceDecal;

	UFUNCTION()
	void ToggleResourceDecalFunc();

	bool bIsMenuModeActive;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartFire();
	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopFire();

	virtual FVector GetPawnViewLocation() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CharacterHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxShield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Shield;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthRegenRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShieldRegenRate;

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	APBWeapon* CurrentWeapon;

	void SetFactionColor(EPBFaction Faction);

	virtual void OnRep_PlayerState() override;

	UPROPERTY(VisibleAnywhere, Replicated, BluePrintReadOnly, Category = "Components")
	UPBHealthComponent* HealthComponent;
	
};

