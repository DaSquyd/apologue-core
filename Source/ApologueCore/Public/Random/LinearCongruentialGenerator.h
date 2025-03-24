// Copyright (c) 2025 David Jacquish

/**
 * https://en.wikipedia.org/wiki/Mersenne_Twister
 */

#pragma once

#include "RandomEngine.h"
#include "LinearCongruentialGenerator.generated.h"

/**
 * https://en.wikipedia.org/wiki/Linear_congruential_generator
 */
USTRUCT(BlueprintType, meta=(DisableSplitPin))
struct APOLOGUECORE_API FLinearCongruentialGenerator : public FRandomEngine
{
	GENERATED_BODY()
	
	static constexpr uint64 Multiplier = 0x5851F42D4C957F2D;
	static constexpr uint64 Increment = 1;
	static constexpr uint64 Modulus = 0xFFFFFFFFFFFFFFFF;
	static constexpr int32 StartingBit = 31;

private:
	UPROPERTY()
	uint64 CurrentSeed = 0;
	
	virtual void Initialize_Implementation(const uint64 Seed) override
	{
		CurrentSeed = Seed;
	}

public:	
	virtual uint32 Random() override
	{
		Mutate();
		return (CurrentSeed << (32 - StartingBit)) >> 32;
	}

	virtual void Discard(const int32 Count) override;

	virtual void Serialize(FArchive& Ar) override
	{
		Super::Serialize(Ar);
		
		Ar << CurrentSeed;
	}
	
private:
	FORCEINLINE void Mutate()
	{
		CurrentSeed = (CurrentSeed * Multiplier + Increment) % Modulus;
	}
};