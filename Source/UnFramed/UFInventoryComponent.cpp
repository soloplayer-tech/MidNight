// Copyright Epic Games, Inc. All Rights Reserved.


#include "UFInventoryComponent.h"

UUFInventoryComponent::UUFInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UUFInventoryComponent::AddItem(FName ItemId, int32 Count)
{
	if (ItemId.IsNone() || Count <= 0)
	{
		return false;
	}

	if (FUFInventoryEntry* ExistingEntry = FindEntry(ItemId))
	{
		ExistingEntry->Count += Count;
	}
	
	else
	{
		FUFInventoryEntry& NewEntry = Items.AddDefaulted_GetRef();
		NewEntry.ItemId = ItemId;
		NewEntry.Count = Count;
	}

	OnInventoryUpdated.Broadcast();
	return true;
}

bool UUFInventoryComponent::RemoveItem(FName ItemId, int32 Count)
{
	if (ItemId.IsNone() || Count <= 0)
	{
		return false;
	}

	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		if (Items[Index].ItemId == ItemId)
		{
			if (Items[Index].Count < Count)
			{
				return false;
			}

			Items[Index].Count -= Count;
			if (Items[Index].Count <= 0)
			{
				Items.RemoveAt(Index);
			}

			OnInventoryUpdated.Broadcast();
			return true;
		}
	}

	return false;
}

bool UUFInventoryComponent::HasItem(FName ItemId) const
{
	return GetItemCount(ItemId) > 0;
}

int32 UUFInventoryComponent::GetItemCount(FName ItemId) const
{
	if (const FUFInventoryEntry* Entry = FindEntry(ItemId))
	{
		return Entry->Count;
	}

	return 0;
}

FUFInventoryEntry* UUFInventoryComponent::FindEntry(FName ItemId)
{
	return Items.FindByPredicate([ItemId](const FUFInventoryEntry& Entry)
	{
		return Entry.ItemId == ItemId;
	});
}

const FUFInventoryEntry* UUFInventoryComponent::FindEntry(FName ItemId) const
{
	return Items.FindByPredicate([ItemId](const FUFInventoryEntry& Entry)
	{
		return Entry.ItemId == ItemId;
	});
}
