// Copyright (c) 2024 David Jacquish

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ApologueStatFunction.generated.h"

class UApologueStat;
class UStatFunctionContext;

/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, CollapseCategories)
class APOLOGUECORE_API UApologueStatFunction : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category="Stat Function")
	int32 GetValue(const TSoftObjectPtr<UApologueStat>& Stat, const int32 BaseValue, const UStatFunctionContext* Context);
};
