// Copyright Epic Games, Inc. All Rights Reserved.

#include "UFPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Museum/UI/MuseumQuizUI.h"
#include "UFPlayerCharacter.h"
#include "UnFramed.h"
#include "UnFramedCameraManager.h"
#include "Widgets/Input/SVirtualJoystick.h"

AUFPlayerController::AUFPlayerController()
{
	PlayerCameraManagerClass = AUnFramedCameraManager::StaticClass();
}

void AUFPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
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
		return;
	}

	if (AUFPlayerCharacter* UFPlayerCharacter = Cast<AUFPlayerCharacter>(InPawn))
	{
		if (!MuseumQuizUI)
		{
			if (MuseumQuizUIClass)
			{
				MuseumQuizUI = CreateWidget<UMuseumQuizUI>(this, MuseumQuizUIClass);

				if (MuseumQuizUI)
				{
					MuseumQuizUI->AddToViewport(0);
					MuseumQuizUI->SetWidgetVisibility(false);
				}
				else
				{
					UE_LOG(LogUnFramed, Error, TEXT("Could not spawn museum quiz UI widget from class '%s'."), *GetNameSafe(MuseumQuizUIClass));
				}
			}
			else
			{
				UE_LOG(LogUnFramed, Warning, TEXT("MuseumQuizUIClass is not set on '%s'. Skipping museum quiz UI creation."), *GetName());
			}
		}

		if (MuseumQuizUI)
		{
			MuseumQuizUI->SetupWithCharacter(UFPlayerCharacter);
		}
	}
}

void AUFPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (IsLocalPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

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
