// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Museum/Interactable.h"
#include "UFCameraPickup.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class AUFPlayerCharacter;

/**
 * World pickup for the player's camera.
 */
UCLASS(Blueprintable)
class UNFRAMED_API AUFCameraPickup : public AInteractable
{
	GENERATED_BODY()

public:

	AUFCameraPickup();

	virtual void OnInteracted_Implementation(APlayerController* InteractingPlayer) override;

	UFUNCTION(BlueprintCallable, Category="Pickup")
	bool Pickup(AUFPlayerCharacter* PickingCharacter);

	UFUNCTION(BlueprintPure, Category="Pickup")
	bool IsAvailable() const { return bAvailable; }

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> CameraMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> InteractionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup")
	bool bDestroyOnPickup = true;

	UFUNCTION(BlueprintImplementableEvent, Category="Pickup", meta=(DisplayName="On Picked Up"))
	void BP_OnPickedUp(AUFPlayerCharacter* PickingCharacter);

private:

	bool bAvailable = true;
};
