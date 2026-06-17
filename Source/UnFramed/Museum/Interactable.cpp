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
	
}
