// Copyright (c) 2024-2025 David Jacquish

#pragma once

#include "CoreMinimal.h"
#include "RandomEngine.h"
#include "UObject/Object.h"
#include "RandomHandlerBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class APOLOGUECORE_API URandomHandlerBase : public UObject
{
	GENERATED_BODY()

public:
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void Initialize(const int64 Seed)
	{
		GetEngine().Initialize(Seed);
	}

	UFUNCTION(BlueprintCallable)
	void InitializeFromString(const FString& Seed);

	UFUNCTION(BlueprintPure)
	bool IsInitialized()
	{
		return GetEngine().IsInitialized();
	}

	UFUNCTION(BlueprintCallable)
	void Reset()
	{
		GetEngine().Reset();
	}

protected:
	virtual FRandomEngine& GetEngine();

public:

	/**
	 * @return Random integer in the range [0... 4294967295]
	*/
	uint32 Random()
	{
		return GetEngine().Random();
	}
	
	/**
	 * @param Max Exclusive
	 * @return Random integer in the range [0... Max)
	*/
	uint32 Random(const uint32 Max)
	{
		return GetEngine().Random(Max);
	}

	/**
	 * @param Min Inclusive
	 * @param Max Inclusive
	 * @return Random integer in the range [Min... Max]
	*/
	int8 RandomRange_Int8(const int8 Min, const int8 Max)
	{
		return GetEngine().RandomRange<int8>(Min, Max);
	}

	/**
	 * @param Min Inclusive
	 * @param Max Inclusive
	 * @return Random integer in the range [Min... Max]
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure="false", Category="ApologueCore|Random", DisplayName="Random Range (byte)")
	uint8 RandomRange_UInt8(const uint8 Min, const uint8 Max)
	{
		return GetEngine().RandomRange<uint8>(Min, Max);
	}

	/**
	 * @param Min Inclusive
	 * @param Max Inclusive
	 * @return Random integer in the range [Min... Max]
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure="false", Category="ApologueCore|Random", DisplayName="Random Range (integer)")
	int32 RandomRange_Int32(const int32 Min, const int32 Max)
	{
		return GetEngine().RandomRange<int32>(Min, Max);
	}

	/**
	 * @param Min Inclusive
	 * @param Max Inclusive
	 * @return Random integer in the range [Min... Max]
	*/
	uint32 RandomRange_UInt32(const uint32 Min, const uint32 Max)
	{
		return GetEngine().RandomRange<uint32>(Min, Max);
	}

	/**
	 * @param Min Inclusive
	 * @param Max Inclusive
	 * @return Random integer in the range [Min... Max]
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure="false", Category="ApologueCore|Random", DisplayName="Random Range (integer64)")
	int64 RandomRange_Int64(const int64 Min, const int64 Max)
	{
		return GetEngine().RandomRange<int64>(Min, Max);
	}

	/**
	 * @param Min Inclusive
	 * @param Max Inclusive
	 * @return Random integer in the range [Min... Max]
	*/
	uint64 RandomRange_UInt64(const uint64 Min, const uint64 Max)
	{
		return GetEngine().RandomRange<uint64>(Min, Max);
	}

	template <typename T>
	T RandomRange(const T Min, const T Max)
	{
		return GetEngine().RandomRange<T>(Min, Max);
	}

	/**
	 * @param Numerator Numerator
	 * @param Denominator Denominator
	 * @return Success
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure="false")
	bool RandomFromFraction(const int32 Numerator, const int32 Denominator)
	{
		return GetEngine().RandomFromFraction(Numerator, Denominator);
	}

	/**
	 * @tparam T Element type
	 * @param Array Array to select from
	 * @return A pointer to a random element within the provided array; nullptr if array was empty
	 */
	template <typename T>
	T* RandomSample(const TArray<T>& Array)
	{
		if (!Array.IsEmpty())
		{
			return &Array[Random(Array.Num())];
		}

		return nullptr;
	}

	/**
	 * @tparam T Element type
	 * @param Array Array to select from
	 * @return A random element within the provided array
	 * @throws std::out_of_range Array is empty
	 */
	template <typename T>
	T& RandomSample(const TArray<T>& Array)
	{
		if (!Array.IsEmpty())
		{
			return Array[Random(Array.Num())];
		}

		throw std::out_of_range("Array is empty");
	}

	UFUNCTION(BlueprintCallable)
	void Discard(const int32 Count)
	{
		return GetEngine().Discard(Count);
	}

	virtual void Serialize(FArchive& Ar) override
	{
		Super::Serialize(Ar);
		
		GetEngine().Serialize(Ar);
	}

	virtual void Serialize(FStructuredArchive::FRecord Record) override
	{
		Super::Serialize(Record);
		
		GetEngine().Serialize(Record);
	}
};
