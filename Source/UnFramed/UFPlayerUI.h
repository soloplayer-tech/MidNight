// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UFPlayerUI.generated.h"

class AUFPlayerCharacter;

/**
 * Default UI bridge for the project player.
 * Forwards sprint meter updates from AUFPlayerCharacter to Blueprint.
 */
UCLASS(abstract)
class UNFRAMED_API UUFPlayerUI : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Sets up delegate listeners for the passed character */
	void SetupCharacter(AUFPlayerCharacter* UFPlayerCharacter);

	/** Called when the character's sprint meter is updated */
	UFUNCTION()
	void OnSprintMeterUpdated(float Percent);

	/** Called when the character's sprint state changes */
	UFUNCTION()
	void OnSprintStateChanged(bool bSprinting);

protected:

	/** Passes control to Blueprint to update the sprint meter widgets */
	UFUNCTION(BlueprintImplementableEvent, Category="UF", meta = (DisplayName = "Sprint Meter Updated"))
	void BP_SprintMeterUpdated(float Percent);

	/** Passes control to Blueprint to update the sprint meter status */
	UFUNCTION(BlueprintImplementableEvent, Category="UF", meta = (DisplayName = "Sprint State Changed"))
	void BP_SprintStateChanged(bool bSprinting);
};
