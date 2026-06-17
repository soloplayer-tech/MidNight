#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

class APlayerController;
class USceneComponent;

UCLASS()
class UNFRAMED_API AInteractable : public AActor
{
	GENERATED_BODY()

public:
	AInteractable();

	
	UFUNCTION(BlueprintNativeEvent, Category="Museum|Artwork")
	void OnInteracted(APlayerController* InteractingPlayer);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> SceneRoot;
};
