// Copyright Epic Games, Inc. All Rights Reserved.


#include "UFPlayerUI.h"
#include "UFPlayerCharacter.h"

void UUFPlayerUI::SetupCharacter(AUFPlayerCharacter* UFPlayerCharacter)
{
	if (!UFPlayerCharacter)
	{
		return;
	}

	UFPlayerCharacter->OnSprintMeterUpdated.AddDynamic(this, &UUFPlayerUI::OnSprintMeterUpdated);
	UFPlayerCharacter->OnSprintStateChanged.AddDynamic(this, &UUFPlayerUI::OnSprintStateChanged);
}

void UUFPlayerUI::OnSprintMeterUpdated(float Percent)
{
	// call the BP handler
	BP_SprintMeterUpdated(Percent);
}

void UUFPlayerUI::OnSprintStateChanged(bool bSprinting)
{
	// call the BP handler
	BP_SprintStateChanged(bSprinting);
}
