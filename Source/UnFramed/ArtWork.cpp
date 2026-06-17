#include "ArtWork.h"

#include "UFInventoryComponent.h"
#include "UFPlayerCharacter.h"
#include "UFPlayerController.h"

AArtWork::AArtWork()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AArtWork::BeginPlay()
{
	Super::BeginPlay();
}

void AArtWork::OnInteracted_Implementation(APlayerController* InteractingPlayer)
{
	Super::OnInteracted_Implementation(InteractingPlayer);

	if (!InteractingPlayer || ArtworkName.IsNone())
	{
		return;
	}

	AUFPlayerController* PlayerController = Cast<AUFPlayerController>(InteractingPlayer);
	AUFPlayerCharacter* PlayerCharacter = Cast<AUFPlayerCharacter>(InteractingPlayer->GetPawn());
	if (!PlayerController || !PlayerCharacter)
	{
		return;
	}

	UUFInventoryComponent* InventoryComponent = PlayerCharacter->GetInventoryComponent();
	if (!InventoryComponent)
	{
		return;
	}

	if (InventoryComponent->HasItem(ArtworkName))
	{
		PlayerController->SetExplain(ArtworkName);
	}
}

void AArtWork::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
