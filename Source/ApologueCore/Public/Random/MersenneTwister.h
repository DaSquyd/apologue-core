// Copyright (c) 2024-2025 David Jacquish

// ReSharper disable CppRedundantCastExpression

#pragma once

#include <random>

#include "RandomEngine.h"
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

public:
	virtual void Initialize_Implementation(const uint64 Seed) override;

	FORCEINLINE int32 GetStateIndex() const
	{
		return ((FEngineType::_Mybase*)&Engine)->_Idx;
	}

	void GetState(TArray<FEngineType::result_type>& Array) const;

	FORCEINLINE void GenerateNewSeed()
	{
		Initialize();
	}

	FORCEINLINE virtual uint32 Random() const override
	{
		return Engine() >> 32;
	}
	
	FORCEINLINE void Discard(const int32 Count) const;

	// Serialization
	friend FArchive& operator<<(FArchive& Ar, FMersenneTwister& MersenneTwister)
	{
		Ar << MersenneTwister.bIsInitialized;
		Ar << MersenneTwister.InitialSeed;
		Ar << *reinterpret_cast<std::_Circ_buf<FEngineType::result_type, MersenneTwister.Engine.state_size>*>(&MersenneTwister.Engine);
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
		Record << SA_VALUE(GET_MEMBER_NAME_STRING_CHECKED(FMersenneTwister, Engine), *reinterpret_cast<FEngineType::_Mybase*>(&MersenneTwister.Engine));
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
