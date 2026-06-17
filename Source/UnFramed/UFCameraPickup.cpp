// Copyright Epic Games, Inc. All Rights Reserved.


#include "UFCameraPickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UFPlayerCharacter.h"
#include "GameFramework/PlayerController.h"

AUFCameraPickup::AUFCameraPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->InitSphereRadius(48.0f);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	SetRootComponent(InteractionSphere);

	CameraMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraMesh"));
	CameraMesh->SetupAttachment(InteractionSphere);
	CameraMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AUFCameraPickup::OnInteracted_Implementation(APlayerController* InteractingPlayer)
{
	Super::OnInteracted_Implementation(InteractingPlayer);

	if (AUFPlayerCharacter* UFPlayerCharacter = InteractingPlayer ? Cast<AUFPlayerCharacter>(InteractingPlayer->GetPawn()) : nullptr)
	{
		Pickup(UFPlayerCharacter);
	}
}

bool AUFCameraPickup::Pickup(AUFPlayerCharacter* PickingCharacter)
{
	if (!bAvailable || !PickingCharacter)
	{
		return false;
	}

	if (!PickingCharacter->PickupCamera(this))
	{
		return false;
	}

	bAvailable = false;
	BP_OnPickedUp(PickingCharacter);

	if (bDestroyOnPickup)
	{
		Destroy();
	}
	else
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}

	return true;
}
