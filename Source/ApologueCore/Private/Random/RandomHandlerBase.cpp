// Copyright (c) 2024-2025 David Jacquish


#include "Random/RandomHandlerBase.h"

#include "Random/RandomUtil.h"

void URandomHandlerBase::Initialize()
{
	Initialize(FRandomUtil::GetSeedFromHardware());
}

void URandomHandlerBase::Initialize_Implementation(const int64 Seed)
{
	unimplemented()
}

void URandomHandlerBase::InitializeFromString(const FString& Seed)
{
	Initialize(FRandomUtil::GetSeedFromString(Seed));
}

bool URandomHandlerBase::IsInitialized_Implementation() const
{
	unimplemented()
	return false;
}

void URandomHandlerBase::Reset_Implementation()
{
	unimplemented()
}

uint32 URandomHandlerBase::Random()
{
	return RandomRange(TNumericLimits<uint32>::Min(), TNumericLimits<uint32>::Max());
}

uint32 URandomHandlerBase::Random(const uint32 Max)
{
	return RandomRange(0U, Max - 1U);
}

int32 URandomHandlerBase::RandomRange(const int32 Min, const int32 Max)
{
	unimplemented();
	return 0;
}

uint32 URandomHandlerBase::RandomRange(const uint32 Min, const uint32 Max)
{
	unimplemented();
	return 0;
}

bool URandomHandlerBase::RandomFromFraction(const int32 Numerator, const int32 Denominator)
{
	return Random(Denominator) < Numerator;
}

void URandomHandlerBase::Discard_Implementation(const int32 Count)
{
	unimplemented();
}
