// Copyright Epic Games, Inc. All Rights Reserved.


#include "UFPlayerCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "UFInventoryComponent.h"
#include "UFCameraPickup.h"

AUFPlayerCharacter::AUFPlayerCharacter()
{
	// create the spotlight
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(GetFirstPersonCameraComponent());

	SpotLight->SetRelativeLocationAndRotation(FVector(30.0f, 17.5f, -5.0f), FRotator(-18.6f, -1.3f, 5.26f));
	SpotLight->Intensity = 0.5;
	SpotLight->SetIntensityUnits(ELightUnits::Lumens);
	SpotLight->AttenuationRadius = 1050.0f;
	SpotLight->InnerConeAngle = 18.7f;
	SpotLight->OuterConeAngle = 45.24f;

	HeldCameraMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeldCameraMesh"));
	HeldCameraMesh->SetupAttachment(GetFirstPersonCameraComponent());
	HeldCameraMesh->SetRelativeLocationAndRotation(FVector(22.0f, 18.0f, -18.0f), FRotator(8.0f, -18.0f, -8.0f));
	HeldCameraMesh->SetRelativeScale3D(FVector(0.65f));
	HeldCameraMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HeldCameraMesh->SetCastShadow(false);
	HeldCameraMesh->SetOnlyOwnerSee(true);
	HeldCameraMesh->SetHiddenInGame(true);

	InventoryComponent = CreateDefaultSubobject<UUFInventoryComponent>(TEXT("InventoryComponent"));
}

void AUFPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// initialize sprint meter to max
	SprintMeter = SprintTime;

	// Initialize the walk speed
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// start the sprint tick timer
	GetWorld()->GetTimerManager().SetTimer(
		SprintTimer,
		this,
		&AUFPlayerCharacter::SprintFixedTick,
		SprintFixedTickTime, true);
}

void AUFPlayerCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// clear the sprint timer
	GetWorld()->GetTimerManager().ClearTimer(SprintTimer);
}

void AUFPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AUFPlayerCharacter::DoStartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AUFPlayerCharacter::DoEndSprint);

		if (CameraAimAction)
		{
			EnhancedInputComponent->BindAction(CameraAimAction, ETriggerEvent::Started, this, &AUFPlayerCharacter::BeginCameraAim);
			EnhancedInputComponent->BindAction(CameraAimAction, ETriggerEvent::Completed, this, &AUFPlayerCharacter::EndCameraAim);
		}

		if (CameraCaptureAction)
		{
			EnhancedInputComponent->BindAction(CameraCaptureAction, ETriggerEvent::Started, this, &AUFPlayerCharacter::CapturePhoto);
		}
	}
}

void AUFPlayerCharacter::DoStartSprint()
{
	// set the sprinting flag
	bSprinting = true;

	// are we out of recovery mode?
	if (!bRecovering)
	{
		// set the sprint walk speed
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

		// call the sprint state changed delegate
		OnSprintStateChanged.Broadcast(true);
	}
}

void AUFPlayerCharacter::DoEndSprint()
{
	// set the sprinting flag
	bSprinting = false;

	// are we out of recovery mode?
	if (!bRecovering)
	{
		// set the default walk speed
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

		// call the sprint state changed delegate
		OnSprintStateChanged.Broadcast(false);
	}
}

void AUFPlayerCharacter::SprintFixedTick()
{
	// are we out of recovery, still have stamina and are moving faster than our walk speed?
	if (bSprinting && !bRecovering && GetVelocity().Length() > WalkSpeed)
	{
		// do we still have meter to burn?
		if (SprintMeter > 0.0f)
		{
			// update the sprint meter
			SprintMeter = FMath::Max(SprintMeter - SprintFixedTickTime, 0.0f);

			// have we run out of stamina?
			if (SprintMeter <= 0.0f)
			{
				// raise the recovering flag
				bRecovering = true;

				// set the recovering walk speed
				GetCharacterMovement()->MaxWalkSpeed = RecoveringWalkSpeed;
			}
		}
	}
	else
	{
		// recover stamina
		SprintMeter = FMath::Min(SprintMeter + SprintFixedTickTime, SprintTime);

		if (SprintMeter >= SprintTime)
		{
			// lower the recovering flag
			bRecovering = false;

			// set the walk or sprint speed depending on whether the sprint button is down
			GetCharacterMovement()->MaxWalkSpeed = bSprinting ? SprintSpeed : WalkSpeed;

			// update the sprint state depending on whether the button is down or not
			OnSprintStateChanged.Broadcast(bSprinting);
		}
	}

	// broadcast the sprint meter updated delegate
	OnSprintMeterUpdated.Broadcast(SprintMeter / SprintTime);
}

void AUFPlayerCharacter::BeginCameraAim()
{
	if (!bHasCamera || bCameraAimActive)
	{
		return;
	}

	bCameraAimActive = true;
	OnCameraAimStateChanged.Broadcast(true);
	BP_OnCameraAimStateChanged(true);
}

void AUFPlayerCharacter::EndCameraAim()
{
	if (!bCameraAimActive)
	{
		return;
	}

	bCameraAimActive = false;
	OnCameraAimStateChanged.Broadcast(false);
	BP_OnCameraAimStateChanged(false);
}

void AUFPlayerCharacter::CapturePhoto()
{
	if (!bHasCamera || !bCameraAimActive)
	{
		return;
	}

	OnCameraCaptured.Broadcast();
	BP_OnCameraPhotoCaptured();
}

bool AUFPlayerCharacter::PickupCamera(AUFCameraPickup* CameraPickup)
{
	if (bHasCamera || !CameraPickup)
	{
		return false;
	}

	bHasCamera = true;
	HeldCameraMesh->SetHiddenInGame(false);
	OnCameraOwnershipChanged.Broadcast(true);
	BP_OnCameraPickedUp();
	return true;
}
