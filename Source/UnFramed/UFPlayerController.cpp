// Copyright Epic Games, Inc. All Rights Reserved.

#include "UFPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Museum/Interactable.h"
#include "UnFramedCameraManager.h"
#include "Museum/UI/MuseumQuizUI.h"
#include "UFPlayerCharacter.h"
#include "UFPlayerUI.h"
#include "Blueprint/UserWidget.h"
#include "UnFramed.h"
#include "UnFramedCameraManager.h"
#include "Widgets/Input/SVirtualJoystick.h"

AUFPlayerController::AUFPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AUnFramedCameraManager::StaticClass();
}

void AUFPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);
		}
		else
		{
			UE_LOG(LogUnFramed, Error, TEXT("Could not spawn mobile controls widget."));
		}
	}
}

void AUFPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!IsLocalPlayerController())
	{
		// set up the UI for the character
		if (AUFPlayerCharacter* UFPlayerCharacter = Cast<AUFPlayerCharacter>(InPawn))
		{
			// create the UI
			if (!PlayerUI)
			{
				if (PlayerUIClass)
				{
					PlayerUI = CreateWidget<UUFPlayerUI>(this, PlayerUIClass);

					if (PlayerUI)
					{
						PlayerUI->AddToViewport(0);
						PlayerUI->SetupCharacter(UFPlayerCharacter);
					}
					else
					{
						UE_LOG(LogUnFramed, Error, TEXT("Could not spawn player UI widget from class '%s'."), *GetNameSafe(PlayerUIClass));
					}
				}
				else
				{
					UE_LOG(LogUnFramed, Warning, TEXT("PlayerUIClass is not set on '%s'. Skipping player UI creation."), *GetName());
				}
			}
		}
	}
}

void AUFPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (OpenInventoryAction)
		{
			EnhancedInputComponent->BindAction(OpenInventoryAction, ETriggerEvent::Started, this, &AUFPlayerController::ToggleInventory);
		}

		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AUFPlayerController::HandleInteract);
		}
	}

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

void AUFPlayerController::ToggleInventory()
{
	SetInventoryOpen(!bInventoryOpen);
}

void AUFPlayerController::SetInventoryOpen(bool bOpen)
{
	bInventoryOpen = bOpen;

	if (!InventoryWidget && InventoryWidgetClass)
	{
		InventoryWidget = CreateWidget<UUserWidget>(this, InventoryWidgetClass);
		if (InventoryWidget)
		{
			InventoryWidget->AddToViewport(10);
			InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (InventoryWidget)
	{
		InventoryWidget->SetVisibility(bInventoryOpen ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	bShowMouseCursor = bInventoryOpen;

	if (bInventoryOpen)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		if (InventoryWidget)
		{
			InputMode.SetWidgetToFocus(InventoryWidget->TakeWidget());
		}
		SetInputMode(InputMode);
	}
	else
	{
		SetInputMode(FInputModeGameOnly());
	}
}

bool AUFPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

bool AUFPlayerController::RequestMuseumQuiz(FName ArtworkName, bool bShowWidget)
{
	if (!MuseumQuizUI)
	{
		UE_LOG(LogUnFramed, Warning, TEXT("MuseumQuizUI is not available on '%s'."), *GetName());
		return false;
	}

	if (bShowWidget)
	{
		ShowMuseumQuizUI();
	}

	return MuseumQuizUI->RequestArtworkQuiz(ArtworkName);
}

void AUFPlayerController::ShowMuseumQuizUI()
{
	if (MuseumQuizUI)
	{
		MuseumQuizUI->SetWidgetVisibility(true);
	}
}

void AUFPlayerController::HideMuseumQuizUI()
{
	if (MuseumQuizUI)
	{
		MuseumQuizUI->SetWidgetVisibility(false);
	}
}

void AUFPlayerController::HandleInteract()
{
	FHitResult HitResult;
	if (AInteractable* InteractableArtwork = TraceInteractableArtwork(HitResult))
	{
		LastInteractedArtwork = InteractableArtwork;
		InteractableArtwork->OnInteracted(this);

		//UE_LOG(LogUnFramed, Log, TEXT("Interacted with artwork '%s'."), *InteractableArtwork->GetArtworkName().ToString());
	}
}

AInteractable* AUFPlayerController::TraceInteractableArtwork(FHitResult& OutHit) const
{
	FVector ViewLocation = FVector::ZeroVector;
	FRotator ViewRotation = FRotator::ZeroRotator;
	GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector TraceStart = ViewLocation;
	const FVector TraceEnd = TraceStart + (ViewRotation.Vector() * InteractTraceDistance);

	TArray<AActor*> ActorsToIgnore;
	if (const APawn* ControlledPawn = GetPawn())
	{
		ActorsToIgnore.Add(const_cast<APawn*>(ControlledPawn));
	}

	const bool bHit = UKismetSystemLibrary::SphereTraceSingle(
		this,
		TraceStart,
		TraceEnd,
		InteractTraceRadius,
		InteractTraceChannel,
		false,
		ActorsToIgnore,
		bDrawInteractTraceDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		OutHit,
		true);

	if (!bHit)
	{
		return nullptr;
	}

	return Cast<AInteractable>(OutHit.GetActor());
}
