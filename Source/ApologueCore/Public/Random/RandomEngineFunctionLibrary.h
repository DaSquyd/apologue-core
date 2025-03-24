// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RandomEngine.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Net/Core/PushModel/PushModel.h"
#include "RandomEngineFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class APOLOGUECORE_API URandomEngineFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	static bool EnsureInitialized(const FRandomEngine& Engine);

#if DO_BLUEPRINT_GUARD
	static void ThrowBlueprintException(const FText& Text);
#endif

public:
	UFUNCTION(BlueprintPure, Category="Apologue|Random")
	static bool GetInitialSeed(UPARAM(Ref) const FRandomEngine& Engine, int64& InitialSeed);

	UFUNCTION(BlueprintPure, Category="Apologue|Random")
	static FORCEINLINE bool IsInitialized(UPARAM(Ref) const FRandomEngine& Engine)
	{
		return Engine.IsInitialized();
	}

	/**
	 * Obtains a random bool.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random", DisplayName="Random Bool")
	static bool RandBool(UPARAM(Ref) FRandomEngine& Engine);

	/**
	 * Obtains a random value in [Min, Max].
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random", DisplayName="Random Range (Integer)")
	static int32 RandRange_Int32(UPARAM(Ref) FRandomEngine& Engine, const int32 Min, const int32 Max);

	/**
	 * Obtains a random value in [Min, Max].
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random", DisplayName="Random Range (Integer64)")
	static int64 RandRange_Int64(UPARAM(Ref) FRandomEngine& Engine, const int64 Min, const int64 Max);

	/**
	 * Obtains a random value in [Min, Max).
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random", DisplayName="Random Range (Float)")
	static double RandRange_Double(UPARAM(Ref) FRandomEngine& Engine, const double Min, const double Max);

	/** 
	 * Gets a random item from specified array (using random stream).
	 * 
	 * @param Engine    		The random stream.
	 * @param TargetArray		The array.
	 * @param OutElement		The random element from this array.
	 * @param OutIndex			The index of random item (will be -1 if array is empty).
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category="Apologue|Random", DisplayName="Random Element in Array",
		meta=(ArrayParm="TargetArray", ArrayTypeDependentParams="OutElement"))
	static FORCEINLINE void RandomElementInArray(UPARAM(Ref) FRandomEngine& Engine, const TArray<int32>& TargetArray, int32& OutElement, int32& OutIndex)
	{
		// see execRandomElementInArray for implementation
		check(0)
	}

	/** 
	 * Gets a random item from specified array (using random stream).
	 * 
	 * @param Engine			The random stream.
	 * @param TargetArray		The array.
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category="Apologue|Random", meta=(ArrayParm="TargetArray", Keywords="random"))
	static FORCEINLINE void ShuffleArray(UPARAM(Ref) FRandomEngine& Engine, const TArray<int32>& TargetArray)
	{
		// see execShuffleArray for implementation
		check(0);
	}

	/**
	 * Obtains a random value in [0.0, 1.0).
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random")
	static double RandomFraction(UPARAM(Ref) FRandomEngine& Engine);

	/**
	 * Returns a random vector of unit size.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random")
	static FVector RandomUnitVector(UPARAM(Ref) FRandomEngine& Engine);

	/**
	 * Returns a random 2D point in a unit circle.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random", DisplayName="Random Point in Unit Circle")
	static FVector2D RandomPointInUnitCircle(UPARAM(Ref) FRandomEngine& Engine);

	/**
	 * Returns a random 3D point in a unit sphere.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random", DisplayName="Random Point in Unit Sphere")
	static FVector RandomPointInUnitSphere(UPARAM(Ref) FRandomEngine& Engine);

	/**
	 * Returns a random point in a bounding box.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random", DisplayName="Random Point in Bounding Box")
	static FVector RandomPointInBoundingBox(UPARAM(Ref) FRandomEngine& Engine, const FVector& Center, const FVector& HalfSize);

	/**
	 * Returns a random point in a box.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random", DisplayName="Random Point in Box")
	static FVector RandomPointInBox(UPARAM(Ref) FRandomEngine& Engine, const FBox& Box);

	/**
	 * Returns a random unit vector, uniformly distributed, within the specified cone.
	 *
	 * @param Engine Engine.
	 * @param Direction The center direction of the cone.
	 * @param HalfAngle Half-angle of cone, in degrees.
	 * @return Normalized vector within the specified cone.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random", DisplayName="Random Cone")
	static FVector RandomCone(UPARAM(Ref) FRandomEngine& Engine, const FVector& Direction, const double HalfAngle);

	/**
	 * Returns a random unit vector, uniformly distributed, within the specified cone.
	 *
	 * @param Engine Engine.
	 * @param Direction The center direction of the cone.
	 * @param HalfAngle Half-angle of cone, in degrees.
	 * @param VerticalHalfAngle Vertical half-angle of cone, in degrees.
	 * @return Normalized vector within the specified cone.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random", DisplayName="Random Cone with Vertical Half Angle")
	static FVector GetConeWithVertical(UPARAM(Ref) FRandomEngine& Engine, const FVector& Direction, const double HalfAngle, const double VerticalHalfAngle);

	UFUNCTION(BlueprintCallable, Category="Apologue|Random", DisplayName="Random from Fraction (Integer)")
	static int32 RandomFromFraction_Int32(UPARAM(Ref) FRandomEngine& Engine, const int32 Numerator, const int32 Denominator);

private:
	// ReSharper disable CppParameterMayBeConstPtrOrRef
	static void GenericRandArray(void* TargetArray, const FArrayProperty* ArrayProperty, FRandomEngine* Engine, void* OutElement, int32* OutIndex);
	static void GenericShuffleArray(void* TargetArray, const FArrayProperty* ArrayProperty, FRandomEngine* Engine);
	// ReSharper restore CppParameterMayBeConstPtrOrRef

	DECLARE_FUNCTION(execRandomElementInArray)
	{
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FProperty>(nullptr);
		FRandomEngine* Engine = reinterpret_cast<FRandomEngine*>(Stack.MostRecentPropertyAddress);

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
			GenericRandArray(ArrayAddr, ArrayProperty, Engine, Result, OutIndex);
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execShuffleArray)
	{
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FProperty>(nullptr);
		FRandomEngine* Engine = reinterpret_cast<FRandomEngine*>(Stack.MostRecentPropertyAddress);

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
			GenericShuffleArray(ArrayAddr, ArrayProperty, Engine);
		P_NATIVE_END;
	}
};
