// Copyright (c) 2024-2025 David Jacquish

#pragma once

#include <random>

#include "CoreMinimal.h"
#include "LinearCongruentialGenerator.h"
#include "RandomHandlerBase.h"
#include "LinearCongruentialGeneratorHandler.generated.h"

/**
 * https://en.wikipedia.org/wiki/Linear_congruential_generator
 */
UCLASS(BlueprintType)
class APOLOGUECORE_API ULinearCongruentialGeneratorHandler : public URandomHandlerBase
{
	GENERATED_BODY()

	UPROPERTY()
	FLinearCongruentialGenerator Generator;

	virtual FRandomEngine& GetEngine() override
	{
		return Generator;
	}
};
