// Copyright (c) 2024-2025 David Jacquish

#pragma once

#include <random>

#include "CoreMinimal.h"
#include "RandomHandlerBase.h"
#include "LinearCongruentialGeneratorHandler.generated.h"

/**
 * 
 */
UCLASS()
class APOLOGUECORE_API ULinearCongruentialGeneratorHandler : public URandomHandlerBase
{
	GENERATED_BODY()

	std::minstd_rand Engine;

	UPROPERTY()
	uint64 InitialSeed;

	UPROPERTY()
	uint64 CurrentSeed;
	
	virtual void Initialize_Implementation(const uint64 InSeed) override
	{
		InitialSeed = InSeed;
		Engine.seed(InitialSeed);
	}

	virtual void Reset_Implementation() override
	{
		Engine.seed(InitialSeed);
	}

	virtual int32 Random_Implementation(const int32 Max) override
	{
		std::uniform_real_distribution Distribution(0, Max - 1);
		CurrentSeed = Distribution(Engine);
		return CurrentSeed;
	}

	virtual void Serialize(FArchive& Ar) override
	{
		Ar << InitialSeed;
		Ar << CurrentSeed;
		
		if (Ar.IsLoading())
		{
			Engine.seed(CurrentSeed);
		}
	}
};
