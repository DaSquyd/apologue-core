// Copyright (c) 2024-2025 David Jacquish

#pragma once

#include "CoreMinimal.h"
#include "MersenneTwister.h"
#include "RandomHandlerBase.h"
#include "MersenneTwisterHandler.generated.h"

/**
 * 
 */
UCLASS()
class APOLOGUECORE_API UMersenneTwisterHandler : public URandomHandlerBase
{
	GENERATED_BODY()

	UPROPERTY()
	FMersenneTwister MersenneTwister;

	FORCEINLINE virtual void Initialize_Implementation(const uint64 Seed) override
	{
		MersenneTwister.Initialize(Seed);
	}

	virtual void Reset_Implementation() override
	{
		MersenneTwister.Reset();
	}

	FORCEINLINE virtual int32 Random_Implementation(const int32 Max) const override
	{
		return MersenneTwister.RandHelper(Max);
	}

	virtual void Serialize(FArchive& Ar) override
	{
		MersenneTwister.Serialize(Ar);
	}

	virtual void Serialize(const FStructuredArchive::FRecord Record) override
	{
		Record << MersenneTwister;
	}
};
