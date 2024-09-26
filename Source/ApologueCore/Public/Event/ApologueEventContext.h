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

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta=(AllowPrivateAccess))
	bool bIsCanceled = false;

public:
	bool IsCanceled() const { return bIsCanceled; }
	void Cancel() { bIsCanceled = true; }
};
