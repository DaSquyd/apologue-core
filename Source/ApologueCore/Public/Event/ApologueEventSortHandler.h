// Copyright (c) 2024 David Jacquish

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ApologueEventSortHandler.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, CollapseCategories)
class APOLOGUECORE_API UApologueEventSortHandler : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Sort(TArray<UObject*>& Objects);
};
