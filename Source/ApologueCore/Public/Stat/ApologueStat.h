// Copyright (c) 2024 David Jacquish

#pragma once

#include "ApologueStatFunction.h"
#include "UObject/Object.h"
#include "Engine/DataAsset.h"
#include "ApologueStat.generated.h"

class UApologueStatFunctionContext;

/**
 * 
 */
UCLASS(BlueprintType)
class APOLOGUECORE_API UApologueStat final : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Apologue Stat", meta=(AllowPrivateAccess))
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category="Apologue Stat", meta=(AllowPrivateAccess, ShowOnlyInnerProperties))
	TObjectPtr<UApologueStatFunction> Function;
	
public:
	const FText& GetDisplayName() const { return DisplayName; }
	
	UFUNCTION(BlueprintPure, Category="Apologue Stat")
	int32 GetValue(const int32 BaseValue, const UApologueStatFunctionContext* Context) const;
};
