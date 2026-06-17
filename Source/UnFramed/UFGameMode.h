// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UFGameMode.generated.h"

/**
 * Default game mode for the project player flow.
 */
UCLASS(abstract)
class UNFRAMED_API AUFGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	/** Constructor */
	AUFGameMode();
};
