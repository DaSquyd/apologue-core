// Copyright (c) 2024-2025 David Jacquish

#pragma once

#include "CoreMinimal.h"
#include "MersenneTwister.h"
#include "RandomHandlerBase.h"
#include "MersenneTwisterHandler.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class APOLOGUECORE_API UMersenneTwisterHandler : public URandomHandlerBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, meta=(allowPrivateAccess))
	FMersenneTwister MersenneTwister;
	
	virtual FRandomEngine* GetEngine() override
	{
		return &MersenneTwister;
	}
};
