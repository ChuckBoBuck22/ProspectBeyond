// Copyright Epic Games, Inc. All Rights Reserved.

#include "PBCharacter.h"
#include "PBPlayerController.h"
#include "PBPlayerState.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "PBWeapon.h"
#include "PB/Public/Components/PBHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "EnhancedInputComponent.h"
#include "Materials/MaterialInstance.h"
#include <EnhancedInput/Public/EnhancedInputSubsystems.h>

//////////////////////////////////////////////////////////////////////////
// APBCharacter

APBCharacter::APBCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	HealthComponent = CreateDefaultSubobject<UPBHealthComponent>(TEXT("HealthComp"));
	APBPlayerState* playerState = Cast<APBPlayerState>(GetPlayerState());
	if (playerState)
	{
		HealthComponent->FactionNumber = StaticCast<uint8, EPBFaction>(playerState->GetFaction());
	}
	

	bIsMenuModeActive = false;

	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	ZoomedFOV = 35.0f;
	ZoomInterpSpeed = 20.0f;

	WeaponAttachSocketName = "WeaponSocket_r";
	MaxHealth = 100.f;
	MaxShield = 100.f;
	HealthRegenRate = 2.5f;
	ShieldRegenRate = 5.f;
 	HealthComponent->DefaultHealth = MaxHealth;
	HealthComponent->HealthRegenRate = HealthRegenRate;
}

void APBCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	// Make sure that we have a valid PlayerController.
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetInputMode(FInputModeGameOnly());
		// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// PawnClientRestart can run more than once in an Actor's lifetime, so start by clearing out any leftover mappings.
			Subsystem->ClearAllMappings();

			// Add each mapping context, along with their priority values. Higher values outprioritize lower values.
			Subsystem->AddMappingContext(InputMappingContextGameplay, 1);
			//Subsystem->AddMappingContext(InputMappingContextMenuMode, 2);
		}
	}
}

void APBCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float targetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
	float newFOV = FMath::FInterpTo(FollowCamera->FieldOfView, targetFOV, DeltaTime, ZoomInterpSpeed);
	

	FollowCamera->SetFieldOfView(newFOV);
}

void APBCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = FollowCamera->FieldOfView;
	HealthComponent->OnHealthChanged.AddDynamic(this, &APBCharacter::OnHealthChanged);

	if (GetLocalRole() == ROLE_Authority)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<APBWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}
	}
}

void APBCharacter::SetFactionColor(EPBFaction Faction)
{
	if (GetMesh() == nullptr || NoFactionMatInst == nullptr) { return; }
	switch (Faction)
	{
	case EPBFaction::EF_NoTeam:
		GetMesh()->SetMaterial(0, NoFactionMatInst);
		HealthComponent->FactionNumber = StaticCast<uint8, EPBFaction>(EPBFaction::EF_NoTeam);
		break;
	case EPBFaction::EF_wildphire:
		GetMesh()->SetMaterial(0, wildphireMatInst);
		HealthComponent->SetFactionNumber(StaticCast<uint8, EPBFaction>(EPBFaction::EF_wildphire));
		ServerSetFaction(StaticCast<uint8, EPBFaction>(EPBFaction::EF_wildphire));
		break;
	case EPBFaction::EF_AlphaAspect:
		GetMesh()->SetMaterial(0, AlphaAspectMatInst);
		HealthComponent->SetFactionNumber(StaticCast<uint8, EPBFaction>(EPBFaction::EF_AlphaAspect));
		ServerSetFaction(StaticCast<uint8, EPBFaction>(EPBFaction::EF_AlphaAspect));
		break;
	case EPBFaction::EF_proclout:
		GetMesh()->SetMaterial(0, procloutMatInst);
		HealthComponent->SetFactionNumber(StaticCast<uint8, EPBFaction>(EPBFaction::EF_proclout));
		ServerSetFaction(StaticCast<uint8, EPBFaction>(EPBFaction::EF_proclout));
		break;
	case EPBFaction::EF_SenseDynamic:
		GetMesh()->SetMaterial(0, SenseDynamicMatInst);
		HealthComponent->SetFactionNumber(StaticCast<uint8, EPBFaction>(EPBFaction::EF_SenseDynamic));
		ServerSetFaction(StaticCast<uint8, EPBFaction>(EPBFaction::EF_SenseDynamic));
		break;
	case EPBFaction::EF_SportingPower:
		GetMesh()->SetMaterial(0, SportingPowerMatInst);
		HealthComponent->SetFactionNumber(StaticCast<uint8, EPBFaction>(EPBFaction::EF_SportingPower));
		ServerSetFaction(StaticCast<uint8, EPBFaction>(EPBFaction::EF_SportingPower));
	case EPBFaction::EF_Protectorate:
		GetMesh()->SetMaterial(0, ProtectorateMatInst);
		HealthComponent->SetFactionNumber(StaticCast<uint8, EPBFaction>(EPBFaction::EF_Protectorate));
		ServerSetFaction(StaticCast<uint8, EPBFaction>(EPBFaction::EF_Protectorate));
		break;
	}
}

void APBCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	APBPlayerState* playerState = GetPlayerState<APBPlayerState>();
	if (playerState)
	{
		SetFactionColor(playerState->GetFaction());
	}
	
}


void APBCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APBCharacter, CurrentWeapon);
	DOREPLIFETIME(APBCharacter, bDied);
	DOREPLIFETIME(APBCharacter, HealthComponent);

}

//////////////////////////////////////////////////////////////////////////
// Input

void APBCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		PlayerEnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APBCharacter::Jump);
		PlayerEnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APBCharacter::StopJumping);
		PlayerEnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &APBCharacter::BeginZoom);
		PlayerEnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Completed, this, &APBCharacter::EndZoom);
		PlayerEnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &APBCharacter::BeginCrouch);
		PlayerEnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APBCharacter::EndCrouch);
		PlayerEnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &APBCharacter::StartFire);
		PlayerEnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &APBCharacter::StopFire);
		PlayerEnhancedInputComponent->BindAction(ToggleResourceDecal, ETriggerEvent::Triggered, this, &APBCharacter::ToggleResourceDecalFunc);
		PlayerEnhancedInputComponent->BindAction(MenuModeAction, ETriggerEvent::Triggered, this, &APBCharacter::ToggleMenuMode);
		PlayerEnhancedInputComponent->BindAction(PlaceBuildingAction, ETriggerEvent::Triggered, this, &APBCharacter::PlaceBuildingInputAction);
		PlayerEnhancedInputComponent->BindAction(PlaceBuildingEscapeAction, ETriggerEvent::Triggered, this, &APBCharacter::PlaceBuildingEscapeInputAction);


		/* Old Input System */
		//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
		//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
		//PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APBCharacter::BeginCrouch);
		//PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APBCharacter::EndCrouch);
		//PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &APBCharacter::BeginZoom);
		//PlayerInputComponent->BindAction("Zoom", IE_Released, this, &APBCharacter::EndZoom);
		//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APBCharacter::StartFire);
		//PlayerInputComponent->BindAction("Fire", IE_Released, this, &APBCharacter::StopFire);


		PlayerInputComponent->BindAxis("Move Forward / Backward", this, &APBCharacter::MoveForward);
		PlayerInputComponent->BindAxis("Move Right / Left", this, &APBCharacter::MoveRight);

		// We have 2 versions of the rotation bindings to handle different kinds of devices differently
		// "turn" handles devices that provide an absolute delta, such as a mouse.
		// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
		PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
		PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &APBCharacter::TurnAtRate);
		PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
		PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &APBCharacter::LookUpAtRate);

		// handle touch devices
		PlayerInputComponent->BindTouch(IE_Pressed, this, &APBCharacter::TouchStarted);
		PlayerInputComponent->BindTouch(IE_Released, this, &APBCharacter::TouchStopped);
	}
	
	
}

void APBCharacter::PlaceBuildingInputAction()
{
	APBPlayerController* PC = Cast<APBPlayerController>(GetController());
	PC->isMousePressed();
}

void APBCharacter::PlaceBuildingEscapeInputAction()
{
	APBPlayerController* PC = Cast<APBPlayerController>(GetController());
	PC->isRightMousePressed();
}

void APBCharacter::ToggleMenuMode()
{
	bIsMenuModeActive = !bIsMenuModeActive;
	APBPlayerController* PC = Cast<APBPlayerController>(GetController());
	PC->ToggleMenuMode();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
	{
		if (bIsMenuModeActive)
		{
			Subsystem->RemoveMappingContext(InputMappingContextGameplay);
			Subsystem->AddMappingContext(InputMappingContextMenuMode, 2);
		}
		else
		{
			Subsystem->RemoveMappingContext(InputMappingContextMenuMode);
			Subsystem->AddMappingContext(InputMappingContextGameplay, 1);
		}
		
	}
}

void APBCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void APBCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void APBCharacter::ToggleResourceDecalFunc()
{
	Cast<APBPlayerController>(GetController())->ToggleResourceDecal();
}

void APBCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void APBCharacter::StopFire()
{
	CurrentWeapon->StopFire();
}

void APBCharacter::OnHealthChanged(UPBHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0 && !bDied)
	{
		bDied = true;
		CurrentWeapon->StopFire();
		ServerDeath();
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DetachFromControllerPendingDestroy();
		SetLifeSpan(10);
		
		
	}
}

void APBCharacter::ServerSetFaction_Implementation(uint8 FactionNum)
{
	HealthComponent->SetFactionNumber(FactionNum);
}

bool APBCharacter::ServerSetFaction_Validate(uint8 FactionNum)
{
	return true;
}

void APBCharacter::ServerDeath_Implementation()
{
	bDied = true;
	SetLifeSpan(10);
}

bool APBCharacter::ServerDeath_Validate()
{
	return true;
}

FVector APBCharacter::GetPawnViewLocation() const
{
	if (GetFollowCamera())
	{
		return this->GetFollowCamera()->GetComponentLocation();
	}
	
	return Super::GetPawnViewLocation();
}

void APBCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void APBCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void APBCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void APBCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void APBCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APBCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void APBCharacter::BeginCrouch()
{
	Crouch();
}

void APBCharacter::EndCrouch()
{
	UnCrouch();
}
