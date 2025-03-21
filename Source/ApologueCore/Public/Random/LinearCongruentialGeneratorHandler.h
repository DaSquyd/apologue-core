// Copyright (c) 2024-2025 David Jacquish

#pragma once

#include <random>

#include "CoreMinimal.h"
#include "RandomHandlerBase.h"
#include "LinearCongruentialGeneratorHandler.generated.h"

/**
 * https://en.wikipedia.org/wiki/Linear_congruential_generator
 */
UCLASS()
class APOLOGUECORE_API ULinearCongruentialGeneratorHandler : public URandomHandlerBase
{
	GENERATED_BODY()

public:
	typedef std::minstd_rand FEngineType;
	static constexpr uint64 Multiplier = 0x5851F42D4C957F2D;
	static constexpr uint64 Increment = 1;
	static constexpr uint64 Modulus = 0xFFFFFFFFFFFFFFFF;
	static constexpr int32 StartingBit = 31;

private:
	UPROPERTY()
	bool bIsInitialized;
	
	UPROPERTY()
	uint64 InitialSeed;

	UPROPERTY()
	uint64 CurrentSeed;
	
	virtual void Initialize_Implementation(const int64 InSeed) override
	{
		InitialSeed = InSeed;
		CurrentSeed = InSeed;
		bIsInitialized = true;
	}

	virtual bool IsInitialized_Implementation() const override
	{
		return bIsInitialized;
	}

	virtual void Reset_Implementation() override
	{
		CurrentSeed = InitialSeed;
	}

public:
	virtual int32 RandomRange(const int32 Min, const int32 Max) override
	{
		return RandomRange_Impl(Min, Max);
	}

	virtual uint32 RandomRange(const uint32 Min, const uint32 Max) override
	{
		return RandomRange_Impl(Min, Max);
	}

private:
	template <typename T>
	T RandomRange_Impl(const T Min, const T Max)
	{
		const uint64 Diff = Max - Min + 1;
		Mutate();
		return ((CurrentSeed << 32 - StartingBit >> 32) * Diff >> 32) + Min;
	}

	virtual void Discard_Implementation(const int32 Count) override
	{
		for (int32 Index = 0; Index < Count; ++Index)
		{
			Mutate();
		}
	}

	void Mutate()
	{
		CurrentSeed = (CurrentSeed * Multiplier + Increment) % Modulus;
	}

public:	
	virtual void Serialize(FArchive& Ar) override
	{
		Ar << bIsInitialized;
		Ar << InitialSeed;
		Ar << CurrentSeed;
	}
};
