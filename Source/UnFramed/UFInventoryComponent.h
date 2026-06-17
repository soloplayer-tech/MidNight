// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UFInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUFInventoryUpdatedDelegate);

USTRUCT(BlueprintType)
struct FUFInventoryEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	FName ItemId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory", meta=(ClampMin=0))
	int32 Count = 0;
};

/**
 * Lightweight player inventory component.
 * Stores simple item ids and counts for the first implementation.
 */
UCLASS(ClassGroup=(Custom), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class UNFRAMED_API UUFInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UUFInventoryComponent();

	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FUFInventoryUpdatedDelegate OnInventoryUpdated;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool AddItem(FName ItemId, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveItem(FName ItemId, int32 Count = 1);

	UFUNCTION(BlueprintPure, Category="Inventory")
	bool HasItem(FName ItemId) const;

	UFUNCTION(BlueprintPure, Category="Inventory")
	int32 GetItemCount(FName ItemId) const;

	UFUNCTION(BlueprintPure, Category="Inventory")
	const TArray<FUFInventoryEntry>& GetItems() const { return Items; }

protected:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Inventory")
	TArray<FUFInventoryEntry> Items;

private:

	FUFInventoryEntry* FindEntry(FName ItemId);
	const FUFInventoryEntry* FindEntry(FName ItemId) const;
};
