// Copyright (c) 2025 David Jacquish

#pragma once
#include "Math/FloatPacker.h"

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

	virtual uint32 Random() const;

	/**
	 * @return A random number in [0..A).
	*/
	uint32 Random(const uint32 A) const;

private:
	template <typename T>
	T GetFraction() const
	{
		return 0.0;
	}

public:
	/**
	 * @return Random number in [0.0, 1.0).
	*/
	template <>
	// ReSharper disable once CppExplicitSpecializationInNonNamespaceScope
	float GetFraction<float>() const
	{
		return GetFraction_Internal<float, 23>();
	}

	/**
	 * @return Random number in [0.0, 1.0).
	*/
	template <>
	// ReSharper disable once CppExplicitSpecializationInNonNamespaceScope
	double GetFraction<double>() const
	{
		return GetFraction_Internal<double, 52>();
	}

private:
	template <typename T, int Mantissa>
	T GetFraction_Internal() const
	{
		checkSlow(bIsInitialized)

		T ReturnValue;

		typedef typename TUnsignedIntType<sizeof(T)>::Type DataType;
		constexpr int32 TotalBits = sizeof(T) * CHAR_BIT;
		constexpr DataType NegativeMask = 1 << (TotalBits - 1);
		constexpr DataType MantissaMask = (DataType(1) << Mantissa) - 1;
		constexpr DataType ExponentMask = ~(NegativeMask | MantissaMask);

		DataType Data = Random();

		constexpr int32 LeftShift = Mantissa - 32;

		if constexpr (LeftShift > 0)
		{
			Data <<= LeftShift;
		}
		else if constexpr (LeftShift < 0)
		{
			Data >>= -LeftShift;
		}

		*reinterpret_cast<uint64*>(&ReturnValue) = ExponentMask | (Data & MantissaMask);

		return ReturnValue - static_cast<T>(1.0);
	}

public:
	/**
	 * @return A random floating point value in [Min, Max).
	*/
	template <typename T>
	typename TEnableIf<TOr<TOrValue<std::is_same_v<T, float>>, TOrValue<std::is_same_v<T, double>>>::Value, T>::Type
	RandomRange(const T Min, const T Max) const
	{
		checkSlow(bIsInitialized)
		return (Max - Min) * GetFraction<T>() + Min;
	}

	/**
	 * @return A random integer value in [Min, Max).
	 */
	template <typename T>
	typename TEnableIf<TOr<TOrValue<std::is_same_v<T, int32>>, TOrValue<std::is_same_v<T, uint32>>>::Value, T>::Type
	RandomRange(const int32 Min, const int32 Max) const
	{
		checkSlow(bIsInitialized)
		checkSlow(Max >= Min)
		return Random(Max - Min + 1u) + Min;
	}

	/**
	 * @param Numerator Numerator
	 * @param Denominator Denominator
	 * @return Success
	*/
	template <typename T>
	typename TEnableIf<TOr<TOrValue<std::is_same_v<T, int32>>, TOrValue<std::is_same_v<T, uint32>>>::Value, bool>::Type
	RandomFromFraction(const T Numerator, const T Denominator) const
	{
		checkSlow(Numerator <= Denominator)
		checkSlow(Denominator > 0)

		return Random(Denominator) < Numerator;
	}

	/**
	 * Returns a random vector of unit size.
	 *
	 * @return Random unit vector.
	 */
	FVector GetUnitVector() const;

	/**
	 * Returns a random point in a 2D unit circle.
	 *
	 * @return Random unit circle point.
	 */
	FVector2D GetPointInUnitCircle() const;

	/**
	 * Returns a random point in a 3D unit sphere.
	 *
	 * @return Random unit sphere point.
	 */
	FVector GetPointInUnitSphere() const;

	FVector GetPointInBoundingBox(const FVector& Center, const FVector& HalfSize) const;

	FORCEINLINE FVector GetPointInBox(const FBox& Box) const;

	/**
	 * Returns a random unit vector, uniformly distributed, within the specified cone.
	 *
	 * @param Dir The center direction of the cone.
	 * @param ConeHalfAngleRad Half-angle of cone, in radians.
	 * @return Normalized vector within the specified cone.
	 */
	FVector GetCone(const FVector& Dir, const double ConeHalfAngleRad) const;

	/**
	 * Returns a random unit vector, uniformly distributed, within the specified cone.
	 *
	 * @param Dir The center direction of the cone.
	 * @param HorizontalConeHalfAngleRad Horizontal half-angle of cone, in radians.
	 * @param VerticalConeHalfAngleRad Vertical half-angle of cone, in radians.
	 * @return Normalized vector within the specified cone.
	 */
	FVector GetCone(const FVector& Dir, const double HorizontalConeHalfAngleRad, const double VerticalConeHalfAngleRad) const;

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
};
