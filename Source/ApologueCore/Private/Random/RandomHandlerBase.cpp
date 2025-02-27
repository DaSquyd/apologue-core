// Copyright (c) 2024-2025 David Jacquish


#include "Random/RandomHandlerBase.h"

#include "Random/RandomUtil.h"

void URandomHandlerBase::Initialize()
{
	Initialize(FRandomUtil::GetSeedFromHardware());
}

void URandomHandlerBase::Initialize_Implementation(const uint64 Seed)
{
	unimplemented()
}

void URandomHandlerBase::InitializeFromString(const FString& Seed)
{
	Initialize(FRandomUtil::GetSeedFromString(Seed));
}

void URandomHandlerBase::Reset_Implementation()
{
	unimplemented()
}

int32 URandomHandlerBase::Random_Implementation(const int32 Max)
{
	unimplemented()
	return 0;
}

int32 URandomHandlerBase::RandomRange(const int32 Min, const int32 Max)
{
	const int32 Diff = Max - Min;
	return Random(Diff) + Min;
}

bool URandomHandlerBase::RandomFromFraction(const int32 Numerator, const int32 Denominator)
{
	return Random(Denominator) < Numerator;
}
