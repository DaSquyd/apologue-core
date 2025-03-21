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

	FORCEINLINE virtual void Initialize_Implementation(const int64 Seed) override
	{
		MersenneTwister.Initialize(Seed);
	}

	FORCEINLINE virtual bool IsInitialized_Implementation() const override
	{
		return MersenneTwister.IsInitialized();
	}

	FORCEINLINE virtual void Reset_Implementation() override
	{
		MersenneTwister.Reset();
	}

	FORCEINLINE virtual int32 RandomRange(const int32 Min, const int32 Max) override
	{
		return MersenneTwister.RandomRange(Min, Max);
	}
	
	FORCEINLINE virtual uint32 RandomRange(const uint32 Min, const uint32 Max) override
	{
		return MersenneTwister.RandomRange(Min, Max);
	}

	FORCEINLINE virtual void Discard_Implementation(const int32 Count) override
	{
		MersenneTwister.Discard(Count);
	}
	
	FORCEINLINE virtual void Serialize(FArchive& Ar) override
	{
		MersenneTwister.Serialize(Ar);
	}
};
