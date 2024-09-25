// Copyright (c) 2024 David Jacquish

#pragma once

#include "CoreMinimal.h"
#include "ApologueStatFunction.h"
#include "UObject/Object.h"
#include "ApologueStat.generated.h"

class UStatFunctionContext;

/**
 * 
 */
UCLASS(BlueprintType)
class APOLOGUECORE_API UApologueStat final : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, meta=(AllowPrivateAccess, ShowOnlyInnerProperties))
	TObjectPtr<UApologueStatFunction> Function;
	
public:
	const FText& GetDisplayName() const { return DisplayName; }
	
	UFUNCTION(BlueprintPure)
	int32 GetValue(const int32 BaseValue, const UStatFunctionContext* Context) const;
};
