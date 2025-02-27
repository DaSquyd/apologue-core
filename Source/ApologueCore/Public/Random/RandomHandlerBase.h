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
	void Initialize(const uint64 Seed);
	
	UFUNCTION(BlueprintCallable, BlueprintPure="false")
	void InitializeFromString(const FString& Seed);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure="false")
	void Reset();
	
	/**
	 * @param Max Exclusive
	 * @return Random integer in the range [0... Max)
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintPure="false")
	int32 Random(const int32 Max);

	/**
	 * @param Min Inclusive
	 * @param Max Exclusive
	 * @return Random integer in the range [Min... Max)
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure="false")
	int32 RandomRange(const int32 Min, const int32 Max);

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
};
