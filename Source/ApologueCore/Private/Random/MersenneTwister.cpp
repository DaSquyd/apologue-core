// ReSharper disable CppRedundantCastExpression

#include "Random/MersenneTwister.h"

#include "Internationalization/Regex.h"
#include "Random/RandomUtil.h"

#define LOCTEXT_NAMESPACE "MersenneTwister"

void FMersenneTwister::Initialize_Implementation(const uint64 Seed)
{
	Engine.seed(Seed);
}

void FMersenneTwister::GetState(TArray<FEngineType::result_type>& Array) const
{
	constexpr int32 StateSize = 2 * FEngineType::state_size;
	Array.Reset(StateSize);

	for (int32 Index = 0; Index < StateSize; ++Index)
	{
		const FEngineType::result_type* State = reinterpret_cast<FEngineType::_Mybase*>(&Engine)->_Ax;
		Array.Add(State[Index]);
	}
}
