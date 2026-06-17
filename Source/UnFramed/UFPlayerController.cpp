// Copyright Epic Games, Inc. All Rights Reserved.

#include "UFPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Museum/Interactable.h"
#include "Museum/MuseumHttpComponent.h"
#include "Museum/UI/MuseumExplainUI.h"
#include "Museum/UI/MuseumQuizUI.h"
#include "UFPlayerCharacter.h"
#include "UFPlayerUI.h"
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
		BindMuseumHttpComponent(UFPlayerCharacter->GetMuseumHttpComponent());

		if (!PlayerUI && PlayerUIClass)
		{
			PlayerUI = CreateWidget<UUFPlayerUI>(this, PlayerUIClass);
			if (PlayerUI)
			{
				PlayerUI->AddToViewport(0);
				PlayerUI->SetupCharacter(UFPlayerCharacter);
			}
		}

		if (!MuseumQuizUI && MuseumQuizUIClass)
		{
			MuseumQuizUI = CreateWidget<UMuseumQuizUI>(this, MuseumQuizUIClass);
			if (MuseumQuizUI)
			{
				MuseumQuizUI->AddToViewport(15);
				MuseumQuizUI->SetWidgetVisibility(false);
				MuseumQuizUI->SetupWithCharacter(UFPlayerCharacter);
			}
		}

		if (!MuseumExplainUI && MuseumExplainUIClass)
		{
			MuseumExplainUI = CreateWidget<UMuseumExplainUI>(this, MuseumExplainUIClass);
			if (MuseumExplainUI)
			{
				MuseumExplainUI->AddToViewport(20);
				MuseumExplainUI->SetWidgetVisibility(false);
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
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void AUFPlayerController::SetExplain(FName ArtworkName)
{
	if (!CachedMuseumHttpComponent)
	{
		UE_LOG(LogUnFramed, Warning, TEXT("MuseumHttpComponent is not available on '%s'."), *GetName());
		return;
	}

	if (MuseumExplainUI)
	{
		MuseumExplainUI->SetArtworkName(ArtworkName);
		MuseumExplainUI->SetExplanationText(TEXT("Loading..."));
		MuseumExplainUI->SetWidgetVisibility(true);
	}

	CachedMuseumHttpComponent->RequestArtworkExplanation(ArtworkName);
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

void AUFPlayerController::HandleExplainRequestSucceeded(FName ArtworkName, FString ExplanationText)
{
	if (!MuseumExplainUI)
	{
		return;
	}

	MuseumExplainUI->SetArtworkName(ArtworkName);
	MuseumExplainUI->SetExplanationText(ExplanationText);
	MuseumExplainUI->SetWidgetVisibility(true);
}

void AUFPlayerController::HandleExplainRequestFailed(FString ErrorMessage)
{
	if (!MuseumExplainUI)
	{
		return;
	}

	MuseumExplainUI->SetErrorText(ErrorMessage);
	MuseumExplainUI->SetWidgetVisibility(true);
}

void AUFPlayerController::BindMuseumHttpComponent(UMuseumHttpComponent* MuseumHttpComponent)
{
	if (CachedMuseumHttpComponent == MuseumHttpComponent)
	{
		return;
	}

	UnbindMuseumHttpComponent();
	CachedMuseumHttpComponent = MuseumHttpComponent;

	if (CachedMuseumHttpComponent)
	{
		CachedMuseumHttpComponent->OnExplainRequestSucceeded.AddDynamic(this, &AUFPlayerController::HandleExplainRequestSucceeded);
		CachedMuseumHttpComponent->OnExplainRequestFailed.AddDynamic(this, &AUFPlayerController::HandleExplainRequestFailed);
	}
}

void AUFPlayerController::UnbindMuseumHttpComponent()
{
	if (CachedMuseumHttpComponent)
	{
		CachedMuseumHttpComponent->OnExplainRequestSucceeded.RemoveDynamic(this, &AUFPlayerController::HandleExplainRequestSucceeded);
		CachedMuseumHttpComponent->OnExplainRequestFailed.RemoveDynamic(this, &AUFPlayerController::HandleExplainRequestFailed);
		CachedMuseumHttpComponent = nullptr;
	}
}
