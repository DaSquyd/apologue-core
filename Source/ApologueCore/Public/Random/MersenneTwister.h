// Copyright (c) 2024-2025 David Jacquish

#pragma once

#include <random>

#include "RandomEngine.h"
#include "MersenneTwister.generated.h"

// Circle Buffer serialization
template <class Type, size_t Nw>
FArchive& operator<<(FArchive& Ar, std::_Circ_buf<Type, Nw>& CircleBuffer)
{
	for (int32 Index = 0; Index < Nw * 2; ++Index)
	{
		Ar << CircleBuffer._Ax[Index];
	}

	Ar << CircleBuffer._Idx;

	return Ar;
}

// Circle Buffer serialization
template <class Type, size_t Nw>
void operator<<(FStructuredArchive::FSlot Slot, std::_Circ_buf<Type, Nw>& CircleBuffer)
{
	for (int32 Index = 0; Index < 2 * Nw; ++Index)
	{
		Slot << CircleBuffer._Ax[Index];
	}

	Slot << CircleBuffer._Idx;
}

/**
 * https://en.wikipedia.org/wiki/Mersenne_Twister
 */
USTRUCT(BlueprintType, meta=(DisableSplitPin))
struct APOLOGUECORE_API FMersenneTwister : public FRandomEngine
{
	GENERATED_BODY()

	// we use 64-bit Mersenne Twister
	typedef std::mt19937_64 FEngineType;

private:
	mutable FEngineType Engine;

	virtual void Initialize_Implementation(const uint64 Seed) override;

public:	
	FORCEINLINE int32 GetStateIndex() const
	{
		// ReSharper disable once CppRedundantCastExpression
		return reinterpret_cast<FEngineType::_Mybase*>(&Engine)->_Idx;
	}

	void GetState(TArray<FEngineType::result_type>& Array) const;

	FORCEINLINE virtual uint32 Random() override
	{
		return Engine() >> 32;
	}
	
	FORCEINLINE virtual void Discard(const int32 Count) override
	{
		Engine.discard(Count);
	}

	// Serialization
	virtual void Serialize(FArchive& Ar) override
	{
		Super::Serialize(Ar);
		Ar << *reinterpret_cast<std::_Circ_buf<FEngineType::result_type, Engine.state_size>*>(&Engine);
	}

	virtual void Serialize(FStructuredArchive::FRecord& Record) override
	{
		Super::Serialize(Record);
		Record << SA_VALUE(GET_MEMBER_NAME_STRING_CHECKED(FMersenneTwister, Engine), *reinterpret_cast<FEngineType::_Mybase*>(&Engine));
	}
};
