// Copyright (c) 2025 David Jacquish

#pragma once
#include "Math/Int128.h"

#include "RandomEngine.generated.h"

USTRUCT(BlueprintType, meta=(DisableSplitPin))
struct APOLOGUECORE_API FRandomEngine
{
	GENERATED_BODY()

private:
	UPROPERTY()
	bool bIsInitialized = false;

	UPROPERTY()
	int64 InitialSeed = 0;

public:
	virtual ~FRandomEngine() = default;

	/**
	 * Initialize with a random seed.
	 */
	void Initialize();

	void Initialize(const uint64 Seed);

	void Initialize(const FString& Seed);

protected:
	virtual void Initialize_Implementation(const uint64 Seed);

public:
	FORCEINLINE bool IsInitialized() const { return bIsInitialized; }

	FORCEINLINE int64 GetInitialSeed() const { return InitialSeed; }

	FORCEINLINE void Reset()
	{
		Initialize(InitialSeed);
	}
	
	virtual uint32 Random();

	template <typename T>
	typename TEnableIf<TIsIntegral<T>::Value, T>::Type
	FORCEINLINE Random()
	{
		checkSlow(bIsInitialized)
		
		if constexpr (sizeof(T) <= 4)
		{
			return Random();
		}
		else if constexpr (sizeof(T) <= 8)
		{
			T Low = Random();
			T High = Random();
			return Low | (High << 32);
		}
		else
		{
			static_assert(0);
			return {};
		}
	}

	/**
	 * @return A random number in [0..A).
	*/
	template <typename T>
	typename TEnableIf<TIsIntegral<T>::Value, T>::Type
	FORCEINLINE Random(const T A)
	{
		checkSlow(bIsInitialized)
		
		checkSlow(!TIsSigned<T>::Value || A > 0)
		
		if constexpr (sizeof(T) <= 4)
		{
			return (static_cast<uint64>(Random()) * A) >> 32;
		}
		else if constexpr (sizeof(T) <= 8)
		{
			FUInt128 Rand(Random<T>());

			FUInt128 LowMultiply = Rand.Multiply(A);
			FUInt128 HighMultiply = Rand.Multiply(A >> 32);

			uint64 ReturnValue = 0;
			ReturnValue += (static_cast<uint64>(HighMultiply.GetQuadPart(2)) + static_cast<uint64>(LowMultiply.GetQuadPart(3))) << 32; // High
			ReturnValue += static_cast<uint64>(HighMultiply.GetQuadPart(1)) + static_cast<uint64>(LowMultiply.GetQuadPart(2)); // Low
			ReturnValue += (static_cast<uint64>(HighMultiply.GetQuadPart(0)) + static_cast<uint64>(LowMultiply.GetQuadPart(1))) >> 32; // Carry
			
			return ReturnValue;
		}
		else
		{
			static_assert(0);
			return {};
		}
	}

	/**
	 * @return A random floating point value in [0.0, 1.0].
	*/
	template <typename T>
	typename TEnableIf<TIsFloatingPoint<T>::Value, T>::Type
	GetFraction()
	{
		if constexpr (sizeof(T) <= 4)
		{
			return ((0x3F800000U) + RandomRange(0, 0x00800000)) - 1.0f;
		}
		else if constexpr (sizeof(T) == 8)
		{
			return (0x3FF0000000000000ULL + RandomRange<uint64>(0, 0x0010000000000000)) - 1.0f;
		}
		else
		{
			static_assert(0);
			return {};
		}
	}

	/**
	 * @return A random floating point value in [Min, Max).
	*/
	template <typename T>
	typename TEnableIf<TIsFloatingPoint<T>::Value, T>::Type
	RandomRange(const T Min, const T Max)
	{
		checkSlow(bIsInitialized)
		return (Max - Min) * GetFraction<T>() + Min;
	}

	/**
	 * @return A random integer point value in [Min, Max].
	*/
	template <typename T>
	typename TEnableIf<TIsIntegral<T>::Value, T>::Type
	RandomRange(const T Min, const T Max)
	{
		checkSlow(bIsInitialized)
		checkSlow(Max >= Min)
		if (Max == TNumericLimits<T>::Max() && Min == TNumericLimits<T>::Min())
		{
			return Random<T>();
		}

		return Random<T>(Max - Min + 1) + Min;
	}

	/**
	 * @param Numerator Numerator
	 * @param Denominator Denominator
	 * @return Success
	*/
	template <typename T>
	typename TEnableIf<TIsIntegral<T>::Value, bool>::Type
	RandomFromFraction(const T Numerator, const T Denominator)
	{
		checkSlow(Numerator <= Denominator)
		checkSlow(Denominator > 0)

		return Random<T>(Denominator) < Numerator;
	}

	/**
	 * Returns a random vector of unit size.
	 *
	 * @return Random unit vector.
	 */
	FVector GetUnitVector();

	/**
	 * Returns a random point in a 2D unit circle.
	 *
	 * @return Random unit circle point.
	 */
	FVector2D GetPointInUnitCircle();

	/**
	 * Returns a random point in a 3D unit sphere.
	 *
	 * @return Random unit sphere point.
	 */
	FVector GetPointInUnitSphere();

	FVector GetPointInBoundingBox(const FVector& Center, const FVector& HalfSize);

	FORCEINLINE FVector GetPointInBox(const FBox& Box);

	/**
	 * Returns a random unit vector, uniformly distributed, within the specified cone.
	 *
	 * @param Dir The center direction of the cone.
	 * @param ConeHalfAngleRad Half-angle of cone, in radians.
	 * @return Normalized vector within the specified cone.
	 */
	FVector GetCone(const FVector& Dir, const double ConeHalfAngleRad);

	/**
	 * Returns a random unit vector, uniformly distributed, within the specified cone.
	 *
	 * @param Dir The center direction of the cone.
	 * @param HorizontalConeHalfAngleRad Horizontal half-angle of cone, in radians.
	 * @param VerticalConeHalfAngleRad Vertical half-angle of cone, in radians.
	 * @return Normalized vector within the specified cone.
	 */
	FVector GetCone(const FVector& Dir, const double HorizontalConeHalfAngleRad, const double VerticalConeHalfAngleRad);

	// Fisher-Yates
	template <typename T>
	void Shuffle(T& List, const typename T::SizeType StartIndex = 0, typename T::SizeType EndIndex = INDEX_NONE)
	{
		checkSlow(StartIndex >= 0)
		checkSlow(StartIndex < EndIndex)
		checkSlow(EndIndex <= List.Num())

		if (EndIndex == INDEX_NONE)
		{
			EndIndex = List.Num();
		}

		for (int32 Index = StartIndex; Index < EndIndex - 1; ++Index)
		{
			CollectionSwap(List, Index, RandomRange(Index, EndIndex - 1));
		}
	}

	// Array Swap
	template <typename T>
	static void CollectionSwap(TArray<T>& Array, const typename TArray<T>::SizeType IndexA,
	                           const typename TArray<T>::SizeType IndexB)
	{
		// ignores sanity checks in TArray::Swap()
		Array.SwapMemory(IndexA, IndexB);
	}

	// Other Container Type Swap
	template <typename T>
	static void CollectionSwap(T& Collection, const typename T::SizeType IndexA, const typename T::SizeType IndexB)
	{
		T Temp = MoveTempIfPossible(Collection[IndexA]);
		Collection[IndexA] = MoveTempIfPossible(Collection[IndexB]);
		Collection[IndexB] = MoveTempIfPossible(Temp);
	}

	virtual void Discard(const int32 Count);

	virtual void Serialize(FArchive& Ar)
	{
		Ar << bIsInitialized;
		Ar << InitialSeed;
	}

	void Serialize(FStructuredArchive::FSlot Slot)
	{
		FStructuredArchive::FRecord Record = Slot.EnterRecord();
		Serialize(Record);
	}

	virtual void Serialize(FStructuredArchive::FRecord& Record)
	{
		Record << SA_VALUE(GET_MEMBER_NAME_STRING_CHECKED(FRandomEngine, bIsInitialized), bIsInitialized);
		Record << SA_VALUE(GET_MEMBER_NAME_STRING_CHECKED(FRandomEngine, InitialSeed), InitialSeed);
	}
};
