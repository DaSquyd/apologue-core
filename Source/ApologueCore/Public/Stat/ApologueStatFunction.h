// Copyright (c) 2024 David Jacquish

#pragma once

#include "CoreMinimal.h"
#include "ApologueStatValue.h"
#include "UObject/Object.h"
#include "ApologueStatFunction.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class APOLOGUECORE_API UApologueStatFunction : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category="Stat Function")
	FApologueStatValue GetValue();
};
