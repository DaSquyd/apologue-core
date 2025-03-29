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

	// Good for flavor text and descriptions
	// Examples: "Strength", "Magic Defense", "Speed"
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Apologue Stat", meta=(AllowPrivateAccess))
	FText VerboseDisplayName;

	// Good for UI
	// Examples: "Strength", "Mg. Defense", "Speed"
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Apologue Stat", meta=(AllowPrivateAccess))
	FText DisplayName;

	// Useful in places where you can only fit very small texts
	// Examples: "STR", "MgD", "SPE"
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Apologue Stat", meta=(AllowPrivateAccess))
	FText AbbreviatedName;

	// Used to calculate the active stat value with provided context
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category="Apologue Stat", meta=(AllowPrivateAccess, ShowOnlyInnerProperties))
	TObjectPtr<UApologueStatFunction> Function;
	
public:
	const FText& GetVerboseDisplayName() const { return VerboseDisplayName; }
	const FText& GetDisplayName() const { return DisplayName; }
	const FText& GetAbbreviatedName() const { return AbbreviatedName; }
	
	UFUNCTION(BlueprintPure, Category="Apologue Stat")
	int32 GetValue(const int32 BaseValue, const UApologueStatFunctionContext* Context) const;
};
