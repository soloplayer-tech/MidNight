// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UFPlayerController.generated.h"

class AInteractable;
class UInputAction;
class UInputMappingContext;
class UMuseumExplainUI;
class UMuseumHttpComponent;
class UMuseumQuizUI;
class UUFPlayerUI;
class UUserWidget;

UCLASS(abstract, config="Game")
class UNFRAMED_API AUFPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category="Museum|UI")
	TSubclassOf<UMuseumExplainUI> MuseumExplainUIClass;

	UPROPERTY()
	TObjectPtr<UMuseumExplainUI> MuseumExplainUI;

	UPROPERTY(EditAnywhere, Category="Museum|UI")
	TSubclassOf<UMuseumQuizUI> MuseumQuizUIClass;

	UPROPERTY()
	TObjectPtr<UMuseumQuizUI> MuseumQuizUI;

	UPROPERTY(EditAnywhere, Category="UF|UI")
	TSubclassOf<UUFPlayerUI> PlayerUIClass;

	UPROPERTY()
	TObjectPtr<UUFPlayerUI> PlayerUI;

	UPROPERTY(EditAnywhere, Category="UF|Inventory")
	TObjectPtr<UInputAction> OpenInventoryAction;

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

	UPROPERTY(EditAnywhere, Category="UF|Inventory")
	TSubclassOf<UUserWidget> InventoryWidgetClass;

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

	UFUNCTION(BlueprintCallable, Category="Museum|UI")
	void SetExplain(FName ArtworkName);

protected:
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	TObjectPtr<UUserWidget> MobileControlsWidget;

	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable, Category="UF|Inventory")
	void ToggleInventory();

	UFUNCTION(BlueprintCallable, Category="UF|Inventory")
	void SetInventoryOpen(bool bOpen);

	UFUNCTION(BlueprintPure, Category="UF|Inventory")
	bool IsInventoryOpen() const { return bInventoryOpen; }

	bool ShouldUseTouchControls() const;

private:
	UFUNCTION()
	void HandleExplainRequestSucceeded(FName ArtworkName, FString ExplanationText);

	UFUNCTION()
	void HandleExplainRequestFailed(FString ErrorMessage);

	void BindMuseumHttpComponent(UMuseumHttpComponent* MuseumHttpComponent);
	void UnbindMuseumHttpComponent();

	UFUNCTION()
	void HandleInteract();

	AInteractable* TraceInteractableArtwork(FHitResult& OutHit) const;

	UPROPERTY(Transient)
	TObjectPtr<AInteractable> LastInteractedArtwork;

	UPROPERTY(Transient)
	TObjectPtr<UMuseumHttpComponent> CachedMuseumHttpComponent;

	bool bInventoryOpen = false;
};
