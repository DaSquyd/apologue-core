// Copyright (c) 2024 David Jacquish

#pragma once

#include "CoreMinimal.h"
#include "ApologueStatValue.h"
#include "UObject/Object.h"
#include "ApologueStat.generated.h"

class UApologueStatFunction;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, DisplayName="Stat")
class APOLOGUECORE_API final UApologueStat : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, meta=(AllowPrivateAccess, ShowOnlyInnerProperties))
	TObjectPtr<UApologueStatFunction> Function;
	
public:
	UFUNCTION(BlueprintPure)
	FApologueStatValue GetValue() const;
};
