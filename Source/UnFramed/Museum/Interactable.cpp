#include "Museum/Interactable.h"

#include "UFPlayerCharacter.h"
#include "Components/SceneComponent.h"

AInteractable::AInteractable()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
}

void AInteractable::OnInteracted_Implementation(APlayerController* InteractingPlayer)
{ 
	// AUFPlayerCharacter* Player = Cast<AUFPlayerCharacter>(InteractingPlayer->GetPawn());
	// if (Player)
	// {
	// 	// 인벤토리 순회 후 같은 이름이 있으면 
	// 	//Player->인벤토리 정보 = ArtworkName;
	// 	//InteractingPlayer->SetExplain();
	// }
}
