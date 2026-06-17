// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UnFramedCharacter.h"
#include "UFPlayerCharacter.generated.h"

class USpotLightComponent;
class UStaticMeshComponent;
class UInputAction;
class UUFInventoryComponent;
class AUFCameraPickup;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUFPlayerSprintMeterUpdatedDelegate, float, Percentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUFPlayerSprintStateChangedDelegate, bool, bSprinting);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUFPlayerCameraStateChangedDelegate, bool, bActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUFPlayerCameraCapturedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUFPlayerCameraOwnershipChangedDelegate, bool, bHasCamera);

/**
 * Default first person player character.
 * Provides stamina-based sprinting.
 */
UCLASS(abstract)
class UNFRAMED_API AUFPlayerCharacter : public AUnFramedCharacter
{
	GENERATED_BODY()

	/** Player light source */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpotLightComponent* SpotLight;

	/** First person held camera mesh shown after the player picks up the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* HeldCameraMesh;

protected:

	/** Sprint input action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* SprintAction;

	/** Enter and exit camera framing mode */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* CameraAimAction;

	/** Capture button while camera mode is active */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* CameraCaptureAction;

	/** If true, we're sprinting */
	bool bSprinting = false;

	/** If true, we're recovering stamina */
	bool bRecovering = false;

	/** Default walk speed when not sprinting or recovering */
	UPROPERTY(EditAnywhere, Category="Walk")
	float WalkSpeed = 250.0f;

	/** Time interval for sprinting stamina ticks */
	UPROPERTY(EditAnywhere, Category="Sprint", meta = (ClampMin = 0, ClampMax = 1, Units = "s"))
	float SprintFixedTickTime = 0.03333f;

	/** Sprint stamina amount. Maxes at SprintTime */
	float SprintMeter = 0.0f;

	/** How long we can sprint for, in seconds */
	UPROPERTY(EditAnywhere, Category="Sprint", meta = (ClampMin = 0, ClampMax = 10, Units = "s"))
	float SprintTime = 3.0f;

	/** Walk speed while sprinting */
	UPROPERTY(EditAnywhere, Category="Sprint", meta = (ClampMin = 0, ClampMax = 10, Units = "cm/s"))
	float SprintSpeed = 600.0f;

	/** Walk speed while recovering stamina */
	UPROPERTY(EditAnywhere, Category="Recovery", meta = (ClampMin = 0, ClampMax = 10, Units = "cm/s"))
	float RecoveringWalkSpeed = 150.0f;

	/** Time it takes for the sprint meter to recover */
	UPROPERTY(EditAnywhere, Category="Recovery", meta = (ClampMin = 0, ClampMax = 10, Units = "s"))
	float RecoveryTime = 0.0f;

	/** Sprint tick timer */
	FTimerHandle SprintTimer;

public:

	/** Delegate called when the sprint meter should be updated */
	FUFPlayerSprintMeterUpdatedDelegate OnSprintMeterUpdated;

	/** Delegate called when we start and stop sprinting */
	FUFPlayerSprintStateChangedDelegate OnSprintStateChanged;

	/** Delegate called when camera framing mode changes */
	FUFPlayerCameraStateChangedDelegate OnCameraAimStateChanged;

	/** Delegate called when the player takes a photo */
	FUFPlayerCameraCapturedDelegate OnCameraCaptured;

	/** Delegate called when the player picks up or loses the camera */
	FUFPlayerCameraOwnershipChangedDelegate OnCameraOwnershipChanged;

	UFUNCTION(BlueprintPure, Category="Inventory")
	UUFInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	UFUNCTION(BlueprintPure, Category="Camera")
	bool HasCamera() const { return bHasCamera; }

	UFUNCTION(BlueprintPure, Category="Camera")
	bool IsCameraAimActive() const { return bCameraAimActive; }

	UFUNCTION(BlueprintPure, Category="Camera")
	UStaticMeshComponent* GetHeldCameraMesh() const { return HeldCameraMesh; }

	UFUNCTION(BlueprintCallable, Category="Camera")
	bool PickupCamera(AUFCameraPickup* CameraPickup);

protected:

	/** Constructor */
	AUFPlayerCharacter();

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Gameplay cleanup */
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	/** Starts sprinting behavior */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoStartSprint();

	/** Stops sprinting behavior */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoEndSprint();

	/** Called while sprinting at a fixed time interval */
	void SprintFixedTick();

	UFUNCTION(BlueprintCallable, Category="Camera")
	void BeginCameraAim();

	UFUNCTION(BlueprintCallable, Category="Camera")
	void EndCameraAim();

	UFUNCTION(BlueprintCallable, Category="Camera")
	void CapturePhoto();

	UFUNCTION(BlueprintImplementableEvent, Category="Camera", meta=(DisplayName="On Camera Picked Up"))
	void BP_OnCameraPickedUp();

	UFUNCTION(BlueprintImplementableEvent, Category="Camera", meta=(DisplayName="On Camera Aim State Changed"))
	void BP_OnCameraAimStateChanged(bool bActive);

	UFUNCTION(BlueprintImplementableEvent, Category="Camera", meta=(DisplayName="On Camera Photo Captured"))
	void BP_OnCameraPhotoCaptured();

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UUFInventoryComponent> InventoryComponent;

	UPROPERTY(EditAnywhere, Category="Interaction", meta=(ClampMin=0, Units="cm"))
	float InteractTraceDistance = 250.0f;

	bool bHasCamera = false;
	bool bCameraAimActive = false;
};
