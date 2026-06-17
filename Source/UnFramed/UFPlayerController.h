// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UFPlayerController.generated.h"

class UInputMappingContext;
class UMuseumQuizUI;
class UUserWidget;

/**
 * Default player controller for the project.
 * Manages input mappings and museum quiz UI.
 */
UCLASS(abstract, config="Game")
class UNFRAMED_API AUFPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category="Museum|UI")
	TSubclassOf<UMuseumQuizUI> MuseumQuizUIClass;

	UPROPERTY()
	TObjectPtr<UMuseumQuizUI> MuseumQuizUI;

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
	bool ShouldUseTouchControls() const;
};
