// Copyright (c) 2024-2025 David Jacquish

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RandomHandlerBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class APOLOGUECORE_API URandomHandlerBase : public UObject
{
	GENERATED_BODY()

public:
	void Initialize();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure="false")
	void Initialize(const int64 Seed);
	
	UFUNCTION(BlueprintCallable, BlueprintPure="false")
	void InitializeFromString(const FString& Seed);

	UFUNCTION(BlueprintPure, BlueprintNativeEvent)
	bool IsInitialized() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure="false")
	void Reset();

	/**
	 * @return Random integer in the range [0... 4294967295]
	*/
	uint32 Random();

	/**
	 * @param Max Exclusive
	 * @return Random integer in the range [0... Max)
	*/
	uint32 Random(const uint32 Max);

	/**
	 * @param Min Inclusive
	 * @param Max Inclusive
	 * @return Random integer in the range [Min... Max]
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure="false")
	virtual int32 RandomRange(const int32 Min, const int32 Max);

	/**
	 * @param Min Inclusive
	 * @param Max Inclusive
	 * @return Random integer in the range [Min... Max]
	*/
	virtual uint32 RandomRange(const uint32 Min, const uint32 Max);

	/**
	 * @param Numerator Numerator
	 * @param Denominator Denominator
	 * @return Success
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure="false")
	bool RandomFromFraction(const int32 Numerator, const int32 Denominator);

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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Discard(const int32 Count);
};
