// Copyright (c) 2024 David Jacquish

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ApologueEventContext.generated.h"

class UApologueEventVariable;

/**
 * Base class for event context.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class APOLOGUECORE_API UApologueEventContext : public UObject
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Apologue Event Context", meta=(AllowPrivateAccess))
	bool bIsCanceled = false;

	UPROPERTY(BlueprintReadOnly, Category="Apologue Event Context", meta=(AllowPrivateAccess))
	int32 Depth = 0;

public:
	bool IsCanceled() const { return bIsCanceled; }
	void Cancel() { bIsCanceled = true; }

	int32 GetDepth() const { return Depth; }

	UFUNCTION(BlueprintCallable, Category="Apologue Event Context")
	UPARAM(DisplayName="New Depth") int32 IncrementDepth() { return ++Depth; }
};
