// Copyright Epic Games, Inc. All Rights Reserved.


#include "UFPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "UnFramedCameraManager.h"
#include "UFPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "UnFramed.h"
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

	// only spawn UI on local player controllers
	if (IsLocalPlayerController())
	{
		// set up the UI for the character
		if (AUFPlayerCharacter* UFPlayerCharacter = Cast<AUFPlayerCharacter>(InPawn))
		{
			// create the UI
			if (!PlayerUI)
			{
				if (PlayerUIClass)
				{
					PlayerUI = CreateWidget<UUserWidget>(this, PlayerUIClass);

					if (PlayerUI)
					{
						PlayerUI->AddToViewport(0);
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

bool AUFPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
