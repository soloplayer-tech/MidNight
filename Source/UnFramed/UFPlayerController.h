// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UFPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

/**
 * Default player controller for the project.
 * Manages input mappings and UI.
 */
UCLASS(abstract, config="Game")
class UNFRAMED_API AUFPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** Type of UI widget to spawn */
	UPROPERTY(EditAnywhere, Category="UF|UI")
	TSubclassOf<UUserWidget> PlayerUIClass;

	/** Pointer to the UI widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> PlayerUI;

public:

	/** Constructor */
	AUFPlayerController();

protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** Gameplay Initialization */
	virtual void BeginPlay() override;

	/** Possessed pawn initialization */
	virtual void OnPossess(APawn* InPawn) override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;
};
