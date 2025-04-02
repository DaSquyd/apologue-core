// Copyright (c) 2024-2025 David Jacquish


#include "Random/RandomHandlerBase.h"

#include "Random/RandomUtil.h"

void URandomHandlerBase::Initialize()
{
	Initialize(FRandomUtil::GetSeedFromHardware());
}

void URandomHandlerBase::InitializeFromString(const FString& Seed)
{
	Initialize(FRandomUtil::GetSeedFromString(Seed));
}

FRandomEngine* URandomHandlerBase::GetEngine()
{
	unimplemented()
	return nullptr;
}
