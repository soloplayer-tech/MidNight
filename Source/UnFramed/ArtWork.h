#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "ArtWork.generated.h"

UCLASS()
class UNFRAMED_API AArtWork : public AInteractable
{
	GENERATED_BODY()

public:
	AArtWork();

	virtual void Tick(float DeltaTime) override;
	virtual void OnInteracted_Implementation(APlayerController* InteractingPlayer) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Museum|Artwork")
	FName ArtworkName;

	UFUNCTION(BlueprintCallable, Category="Museum|Artwork")
	FName GetArtworkName() const { return ArtworkName; }
};
