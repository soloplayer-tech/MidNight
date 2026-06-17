// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UFPlayerController.generated.h"

class UInputMappingContext;
class UMuseumQuizUI;
class UInputAction;
class UUserWidget;
class UUFPlayerUI;
class AInteractable;

/**
 * Default player controller for the project.
 * Manages input mappings and UI.
 */
UCLASS(abstract, config="Game")
class UNFRAMED_API AUFPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category="Museum|UI")
	TSubclassOf<UMuseumQuizUI> MuseumQuizUIClass;
	
	/** Pointer to the UI widget */
	UPROPERTY()
	TObjectPtr<UMuseumQuizUI> MuseumQuizUI;

	/** Type of UI widget to spawn */
	UPROPERTY(EditAnywhere, Category="UF|UI")
	TSubclassOf<UUFPlayerUI> PlayerUIClass;
	
	UPROPERTY()
	TObjectPtr<UUFPlayerUI> PlayerUI;

	
	

	/** Inventory toggle input action */
	UPROPERTY(EditAnywhere, Category="UF|Inventory")
	TObjectPtr<UInputAction> OpenInventoryAction;

	/** Interact input action */
	UPROPERTY(EditAnywhere, Category="UF|Interaction")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, Category="UF|Interaction", meta=(ClampMin="0.0"))
	float InteractTraceDistance = 350.0f;

	UPROPERTY(EditAnywhere, Category="UF|Interaction", meta=(ClampMin="0.0"))
	float InteractTraceRadius = 35.0f;

	UPROPERTY(EditAnywhere, Category="UF|Interaction")
	TEnumAsByte<ETraceTypeQuery> InteractTraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	UPROPERTY(EditAnywhere, Category="UF|Interaction")
	bool bDrawInteractTraceDebug = false;

	/** Inventory widget class */
	UPROPERTY(EditAnywhere, Category="UF|Inventory")
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	/** Inventory widget instance */
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> InventoryWidget;

public:
	AUFPlayerController();

	UFUNCTION(BlueprintCallable, Category="Museum|UI")
	UMuseumQuizUI* GetMuseumQuizUI() const { return MuseumQuizUI; }

	UFUNCTION(BlueprintCallable, Category="Museum|UI")
	bool RequestMuseumQuiz(FName ArtworkName, bool bShowWidget = true);

	UFUNCTION(BlueprintCallable, Category="Museum|UI")
	void ShowMuseumQuizUI();

	UFUNCTION(BlueprintCallable, Category="Museum|UI")
	void HideMuseumQuizUI();

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

	UFUNCTION(BlueprintCallable, Category="UF|Inventory")
	void ToggleInventory();

	UFUNCTION(BlueprintCallable, Category="UF|Inventory")
	void SetInventoryOpen(bool bOpen);

	UFUNCTION(BlueprintPure, Category="UF|Inventory")
	bool IsInventoryOpen() const { return bInventoryOpen; }

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;

private:
	UFUNCTION()
	void HandleInteract();

	AInteractable* TraceInteractableArtwork(FHitResult& OutHit) const;

	UPROPERTY(Transient)
	TObjectPtr<AInteractable> LastInteractedArtwork;

	bool bInventoryOpen = false;
};
