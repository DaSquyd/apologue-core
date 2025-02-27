// Copyright (c) 2024 David Jacquish

#pragma once

#include <random>

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Net/Core/PushModel/PushModel.h"
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

USTRUCT(BlueprintType, meta=(DisableSplitPin))
struct APOLOGUECORE_API FMersenneTwister
{
	GENERATED_BODY()

	typedef std::mt19937_64 FEngineType;

private:
	mutable FEngineType Engine;

	UPROPERTY()
	int64 InitialSeed = 0;

	UPROPERTY()
	bool bIsInitialized = false;

public:
	/**
	 * Initialize with a random seed.
	 */
	void Initialize();

	void Initialize(const uint64 Seed);

	void Initialize(const FString& Seed);

	/**
	 * Resets the mersenne twister back to the state from the initial seed.
	 */
	// ReSharper disable once CppMemberFunctionMayBeConst
	FORCEINLINE void Reset() const
	{
		Engine.seed(InitialSeed);
	}

	FORCEINLINE int64 GetInitialSeed() const { return InitialSeed; }

	FORCEINLINE int32 GetStateIndex() const { return Engine._Idx; }

	FORCEINLINE bool IsInitialized() const { return bIsInitialized; }

	void GetState(TArray<FEngineType::result_type>& Array) const;

	FORCEINLINE void GenerateNewSeed()
	{
		Initialize();
	}

	/**
	 * Helper function for rand implementations.
	 *
	 * @return A random number in [0..A).
	*/
	template <typename T>
	typename TEnableIf<TIsArithmetic<T>::Value && !TIsFloatingPoint<T>::Value, T>::Type
	RandHelper(const T A) const
	{
		ensure(bIsInitialized);
		return RandomRange<T>(0, A - 1);
	}

	/**
	 * @return A random floating point value in [Min, Max).
	 */
	template <typename T>
	typename TEnableIf<TIsFloatingPoint<T>::Value, T>::Type
	RandomRange(const T Min, const T Max) const
	{
		ensure(bIsInitialized);
		std::uniform_real_distribution<T> Distribution(Min, Max);
		const T Value = Distribution(Engine);
		return Value;
	}

	/**
	 * @return A random integer value in [Min, Max].
	 */
	template <typename T>
	typename TEnableIf<TIsArithmetic<T>::Value && !TIsFloatingPoint<T>::Value, T>::Type
	RandomRange(const T Min, const T Max) const
	{
		ensure(bIsInitialized);
		std::uniform_int_distribution<T> Distribution(Min, Max);
		const T Value = Distribution(Engine);
		return Value;
	}

	/**
	 * @return Random number in [0.0, 1.0).
	 */
	template <typename T>
	typename TEnableIf<TIsFloatingPoint<T>::Value, T>::Type
	GetFraction() const
	{
		return RandomRange<T>(0, 1);
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

	/**
	 * @param Numerator Numerator
	 * @param Denominator Denominator
	 * @return Success
	*/
	template <typename T>
	typename TEnableIf<TIsArithmetic<T>::Value && !TIsFloatingPoint<T>::Value, bool>::Type
	RandomFromFraction(const T Numerator, const T Denominator) const
	{
		return RandHelper(Denominator) < Numerator;
	}

	/**
	 * A version of RandomFromFraction that throws an error if Numerator or Denominator is out of range.
	 * @param Numerator Numerator
	 * @param Denominator Denominator
	 * @return Success
	*/
	bool RandomFromFractionChecked(const int32 Numerator, const int32 Denominator) const;

	// Fisher-Yates
	template <typename T>
	void Shuffle(T& List, const typename T::SizeType StartIndex = 0, typename T::SizeType EndIndex = INDEX_NONE)
	{
		check(StartIndex >= 0)
		check(StartIndex < EndIndex)
		check(EndIndex <= List.Num())

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

	// Serialization
	friend FArchive& operator<<(FArchive& Ar, FMersenneTwister& MersenneTwister)
	{
		Ar << MersenneTwister.bIsInitialized;
		Ar << MersenneTwister.InitialSeed;
		Ar << MersenneTwister.Engine;
		return Ar;
	}

	bool Serialize(FArchive& Ar)
	{
		Ar << *this;
		return true;
	}

	friend void operator<<(FStructuredArchive::FSlot Slot, FMersenneTwister& MersenneTwister)
	{
		const FStructuredArchive::FRecord Record = Slot.EnterRecord();
		Record << MersenneTwister;
	}

	bool Serialize(const FStructuredArchive::FSlot Slot)
	{
		Slot << *this;
		return true;
	}

	friend void operator<<(FStructuredArchive::FRecord Record, FMersenneTwister& MersenneTwister)
	{
		Record << SA_VALUE(GET_MEMBER_NAME_STRING_CHECKED(FMersenneTwister, bIsInitialized), MersenneTwister.bIsInitialized);
		Record << SA_VALUE(GET_MEMBER_NAME_STRING_CHECKED(FMersenneTwister, InitialSeed), MersenneTwister.InitialSeed);
		Record << SA_VALUE(GET_MEMBER_NAME_STRING_CHECKED(FMersenneTwister, Engine), MersenneTwister.Engine);
	}

	bool Serialize(const FStructuredArchive::FRecord Record)
	{
		Record << *this;
		return true;
	}
};

const FName RandomAccessToEmptyArrayWarning = FName("RandomAccessToEmptyArrayWarning");

UCLASS()
class APOLOGUECORE_API UMersenneTwisterLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	static bool EnsureInitialized(const FMersenneTwister& MersenneTwister);

#if DO_BLUEPRINT_GUARD
	static void ThrowBlueprintException(const FText& Text);
#endif
	
public:
	UFUNCTION(BlueprintPure, Category="Apologue|Random|Mersenne Twister")
	static bool GetInitialSeed(UPARAM(Ref) const FMersenneTwister& MersenneTwister, int64& InitialSeed);

	UFUNCTION(BlueprintPure, Category="Apologue|Random|Mersenne Twister")
	static bool GetState(UPARAM(Ref) const FMersenneTwister& MersenneTwister, TArray<int64>& State, int32& Index);

	UFUNCTION(BlueprintPure, Category="Apologue|Random|Mersenne Twister")
	static FORCEINLINE bool IsInitialized(UPARAM(Ref) const FMersenneTwister& MersenneTwister)
	{
		return MersenneTwister.IsInitialized();
	}

	/**
	 * Obtains a random bool.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Bool")
	static bool RandBool(UPARAM(Ref) const FMersenneTwister& MersenneTwister);

	/**
	 * Obtains a random value in [Min, Max].
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Range (Integer)")
	static int32 RandRange_Int32(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const int32 Min, const int32 Max);

	/**
	 * Obtains a random value in [Min, Max].
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Range (Integer64)")
	static int64 RandRange_Int64(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const int64 Min, const int64 Max);

	/**
	 * Obtains a random value in [Min, Max).
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Range (Float)")
	static double RandRange_Double(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const double Min, const double Max);

	/** 
	 * Gets a random item from specified array (using random stream).
	 * 
	 * @param MersenneTwister	The random stream.
	 * @param TargetArray		The array.
	 * @param OutElement		The random element from this array.
	 * @param OutIndex			The index of random item (will be -1 if array is empty).
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Element in Array",
		meta=(ArrayParm="TargetArray", ArrayTypeDependentParams="OutElement"))
	static FORCEINLINE void RandomElementInArray(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const TArray<int32>& TargetArray, int32& OutElement, int32& OutIndex)
	{
		// see execRandomElementInArray for implementation
		check(0)
	}

	/** 
	 * Gets a random item from specified array (using random stream).
	 * 
	 * @param MersenneTwister	The random stream.
	 * @param TargetArray		The array.
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category="Apologue|Random|Mersenne Twister", meta=(ArrayParm="TargetArray", Keywords="random"))
	static FORCEINLINE void ShuffleArray(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const TArray<int32>& TargetArray)
	{
		// see execShuffleArray for implementation
		check(0);
	}

	/**
	 * Obtains a random value in [0.0, 1.0).
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister")
	static double RandomFraction(UPARAM(Ref) const FMersenneTwister& MersenneTwister);

	/**
	 * Returns a random vector of unit size.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister")
	static FVector RandomUnitVector(UPARAM(Ref) const FMersenneTwister& MersenneTwister);

	/**
	 * Returns a random 2D point in a unit circle.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Point in Unit Circle")
	static FVector2D RandomPointInUnitCircle(UPARAM(Ref) const FMersenneTwister& MersenneTwister);

	/**
	 * Returns a random 3D point in a unit sphere.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Point in Unit Sphere")
	static FVector RandomPointInUnitSphere(UPARAM(Ref) const FMersenneTwister& MersenneTwister);

	/**
	 * Returns a random point in a bounding box.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Point in Bounding Box")
	static FVector RandomPointInBoundingBox(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const FVector& Center, const FVector& HalfSize);

	/**
	 * Returns a random point in a box.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Point in Box")
	static FVector RandomPointInBox(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const FBox& Box);

	/**
	 * Returns a random unit vector, uniformly distributed, within the specified cone.
	 *
	 * @param MersenneTwister MersenneTwister.
	 * @param Direction The center direction of the cone.
	 * @param HalfAngle Half-angle of cone, in degrees.
	 * @return Normalized vector within the specified cone.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Cone")
	static FVector RandomCone(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const FVector& Direction, const double HalfAngle);

	/**
	 * Returns a random unit vector, uniformly distributed, within the specified cone.
	 *
	 * @param MersenneTwister MersenneTwister.
	 * @param Direction The center direction of the cone.
	 * @param HalfAngle Half-angle of cone, in degrees.
	 * @param VerticalHalfAngle Vertical half-angle of cone, in degrees.
	 * @return Normalized vector within the specified cone.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Cone with Vertical Half Angle")
	static FVector GetConeWithVertical(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const FVector& Direction, const double HalfAngle,
	                                               const double VerticalHalfAngle);

	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random from Fraction (Integer)")
	static int32 RandomFromFraction_Int32(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const int32 Numerator, const int32 Denominator);

	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random from Fraction (Integer64)")
	static int64 RandomFromFraction_Int64(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const int64 Numerator, const int64 Denominator);

private:
	// ReSharper disable CppParameterMayBeConstPtrOrRef
	static void GenericRandArray(void* TargetArray, const FArrayProperty* ArrayProperty, FMersenneTwister* MersenneTwister, void* OutElement, int32* OutIndex);
	static void GenericShuffleArray(void* TargetArray, const FArrayProperty* ArrayProperty, FMersenneTwister* MersenneTwister);
	// ReSharper restore CppParameterMayBeConstPtrOrRef
	
	DECLARE_FUNCTION(execRandomElementInArray)
	{
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FProperty>(nullptr);
		FMersenneTwister* MersenneTwister = reinterpret_cast<FMersenneTwister*>(Stack.MostRecentPropertyAddress);

		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FArrayProperty>(nullptr);
		void* ArrayAddr = Stack.MostRecentPropertyAddress;
		const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);
		if (!ArrayProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}

		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FProperty>(nullptr);
		void* Result = Stack.MostRecentPropertyAddress;

		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FProperty>(nullptr);
		int32* OutIndex = reinterpret_cast<int32*>(Stack.MostRecentPropertyAddress);

		P_FINISH;
		P_NATIVE_BEGIN;
			GenericRandArray(ArrayAddr, ArrayProperty, MersenneTwister, Result, OutIndex);
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execShuffleArray)
	{
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FProperty>(nullptr);
		FMersenneTwister* MersenneTwister = reinterpret_cast<FMersenneTwister*>(Stack.MostRecentPropertyAddress);

		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FArrayProperty>(nullptr);
		void* ArrayAddr = Stack.MostRecentPropertyAddress;
		const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);
		if (!ArrayProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}

		P_FINISH;
		P_NATIVE_BEGIN;
			MARK_PROPERTY_DIRTY(Stack.Object, ArrayProperty);
			GenericShuffleArray(ArrayAddr, ArrayProperty, MersenneTwister);
		P_NATIVE_END;
	}
};
