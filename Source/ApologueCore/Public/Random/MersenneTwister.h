// Copyright (c) 2024 David Jacquish

#pragma once

#include <random>

#include "CoreMinimal.h"
#include "Blueprint/BlueprintExceptionInfo.h"
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
	FORCEINLINE void Initialize()
	{
		// hardware randomization
		// TODO: Is this the best way to handle this across all devices?
		std::random_device RandomDevice;
		InitialSeed = RandomDevice();
		Engine.seed(InitialSeed);
		bIsInitialized = true;
	}

	FORCEINLINE void Initialize(const uint64 Seed)
	{
		InitialSeed = Seed;
		Engine.seed(Seed);
		bIsInitialized = true;
	}

	void Initialize(const FString& Seed)
	{
		if (Seed.IsEmpty())
		{
			// Random initialization
			Initialize();
			return;
		}

		// Explicitly check for 0 or -1 because these are potential fail values for FCString::Strtoi64() and we don't want any false negatives.
		const FRegexPattern ZeroPattern(TEXT(R"(^(\+|-)?0+$)"));
		FRegexMatcher ZeroMatcher(ZeroPattern, Seed);
		if (ZeroMatcher.FindNext())
		{
			Initialize(0);
			return;
		}

		const FRegexPattern NegativeOnePattern(TEXT(R"(^-[0]*1$)"));
		FRegexMatcher NegativeOneMatcher(ZeroPattern, Seed);
		if (NegativeOneMatcher.FindNext())
		{
			Initialize(-1);
			return;
		}

		const int64 IntSeed = FCString::Strtoi64(*Seed, nullptr, 10);
		if (IntSeed != 0 && IntSeed != -1)
		{
			Initialize(IntSeed);
			return;
		}

		const uint32 HashSeed = GetTypeHash(Seed);
		Initialize(HashSeed);
	}

	/**
	 * Resets the mersenne twister back to the state from the initial seed.
	 */
	// ReSharper disable once CppMemberFunctionMayBeConst
	FORCEINLINE void Reset()
	{
		Engine.seed(InitialSeed);
	}

	FORCEINLINE int64 GetInitialSeed() const
	{
		return InitialSeed;
	}

	FORCEINLINE int32 GetStateIndex() const
	{
		return Engine._Idx;
	}

	FORCEINLINE bool IsInitialized() const
	{
		return bIsInitialized;
	}

	void GetState(TArray<FEngineType::result_type>& Array) const
	{
		constexpr int32 StateSize = 2 * FEngineType::state_size;
		Array.Reset(StateSize);

		for (int32 Index = 0; Index < StateSize; ++Index)
		{
			Array.Add(Engine._Ax[Index]);
		}
	}

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
	FORCEINLINE typename TEnableIf<TIsArithmetic<T>::Value && !TIsFloatingPoint<T>::Value, T>::Type
	RandHelper(const T A) const
	{
		ensure(bIsInitialized);
		return RandomRange<T>(0, A - 1);
	}

	/**
	 * @return A random floating point value in [Min, Max).
	 */
	template <typename T>
	FORCEINLINE typename TEnableIf<TIsFloatingPoint<T>::Value, T>::Type
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
	FORCEINLINE typename TEnableIf<TIsArithmetic<T>::Value && !TIsFloatingPoint<T>::Value, T>::Type
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
	FORCEINLINE typename TEnableIf<TIsFloatingPoint<T>::Value, T>::Type
	GetFraction() const
	{
		return RandomRange<T>(0, 1);
	}

	/**
	 * Returns a random vector of unit size.
	 *
	 * @return Random unit vector.
	 */
	FORCEINLINE FVector GetUnitVector() const
	{
		FVector Result;
		FVector::FReal Length;

		do
		{
			// Check random vectors in the unit sphere so result is statistically uniform.
			Result.X = RandomRange(-1.0, 1.0);
			Result.Y = RandomRange(-1.0, 1.0);
			Result.Z = RandomRange(-1.0, 1.0);
			Length = Result.SizeSquared();
		}
		while (Length > 1.f || Length < UE_KINDA_SMALL_NUMBER);

		return Result.GetUnsafeNormal();
	}

	/**
	 * Returns a random point in a 2D unit circle.
	 *
	 * @return Random unit circle point.
	 */
	FORCEINLINE FVector2D GetPointInUnitCircle() const
	{
		FVector2D Result;
		FVector2D::FReal Length;

		do
		{
			Result.X = RandomRange(-1.0, 1.0);
			Result.Y = RandomRange(-1.0, 1.0);
			Length = Result.SizeSquared();
		}
		while (Length > 1.f);

		return Result;
	}

	/**
	 * Returns a random point in a 3D unit sphere.
	 *
	 * @return Random unit sphere point.
	 */
	FORCEINLINE FVector GetPointInUnitSphere() const
	{
		FVector Result;
		FVector::FReal Length;

		do
		{
			Result.X = RandomRange(-1.0, 1.0);
			Result.Y = RandomRange(-1.0, 1.0);
			Result.Z = RandomRange(-1.0, 1.0);
			Length = Result.SizeSquared();
		}
		while (Length > 1.f);

		return Result;
	}

	FORCEINLINE FVector GetPointInBoundingBox(const FVector& Center, const FVector& HalfSize) const
	{
		const FVector BoxMin = Center - HalfSize;
		const FVector BoxMax = Center + HalfSize;
		return GetPointInBox(FBox(BoxMin, BoxMax));
	}

	FORCEINLINE FVector GetPointInBox(const FBox& Box) const
	{
		return FVector(RandomRange(Box.Min.X, Box.Max.X),
		               RandomRange(Box.Min.Y, Box.Max.Y),
		               RandomRange(Box.Min.Z, Box.Max.Z));
	}

	/**
	 * Returns a random unit vector, uniformly distributed, within the specified cone.
	 *
	 * @param Dir The center direction of the cone.
	 * @param ConeHalfAngleRad Half-angle of cone, in radians.
	 * @return Normalized vector within the specified cone.
	 */
	FORCEINLINE FVector GetCone(const FVector& Dir, const double ConeHalfAngleRad) const
	{
		if (ConeHalfAngleRad > 0.f)
		{
			const double RandU = GetFraction<double>();
			const double RandV = GetFraction<double>();

			// Get spherical coords that have an even distribution over the unit sphere
			// Method described at http://mathworld.wolfram.com/SpherePointPicking.html	
			const double Theta = 2.0 * UE_DOUBLE_PI * RandU;
			double Phi = FMath::Acos(2.0 * RandV - 1.0);

			// restrict phi to [0, ConeHalfAngleRad]
			// this gives an even distribution of points on the surface of the cone
			// centered at the origin, pointing upward (z), with the desired angle
			Phi = FMath::Fmod(Phi, ConeHalfAngleRad);

			// get axes we need to rotate around
			const FMatrix DirMat = FRotationMatrix(Dir.Rotation());
			// note the axis translation, since we want the variation to be around X
			const FVector DirZ = DirMat.GetUnitAxis(EAxis::X);
			const FVector DirY = DirMat.GetUnitAxis(EAxis::Y);

			FVector Result = Dir.RotateAngleAxis(FMath::RadiansToDegrees(Phi), DirY);
			Result = Result.RotateAngleAxis(FMath::RadiansToDegrees(Theta), DirZ);

			// ensure it's a unit vector (might not have been passed in that way)
			Result = Result.GetSafeNormal();

			return Result;
		}

		return Dir.GetSafeNormal();
	}

	/**
	 * Returns a random unit vector, uniformly distributed, within the specified cone.
	 *
	 * @param Dir The center direction of the cone.
	 * @param HorizontalConeHalfAngleRad Horizontal half-angle of cone, in radians.
	 * @param VerticalConeHalfAngleRad Vertical half-angle of cone, in radians.
	 * @return Normalized vector within the specified cone.
	 */
	FORCEINLINE FVector GetCone(const FVector& Dir, const double HorizontalConeHalfAngleRad, const double VerticalConeHalfAngleRad) const
	{
		if (VerticalConeHalfAngleRad > 0.0 && HorizontalConeHalfAngleRad > 0.0)
		{
			const double RandU = GetFraction<double>();
			const double RandV = GetFraction<double>();

			// Get spherical coords that have an even distribution over the unit sphere
			// Method described at http://mathworld.wolfram.com/SpherePointPicking.html	
			const double Theta = 2.0 * UE_DOUBLE_PI * RandU;
			double Phi = FMath::Acos(2.0 * RandV - 1.0);

			// restrict phi to [0, ConeHalfAngleRad]
			// where ConeHalfAngleRad is now a function of Theta
			// (specifically, radius of an ellipse as a function of angle)
			// function is ellipse function (x/a)^2 + (y/b)^2 = 1, converted to polar coords
			double ConeHalfAngleRad = FMath::Square(FMath::Cos(Theta) / VerticalConeHalfAngleRad) + FMath::Square(FMath::Sin(Theta) / HorizontalConeHalfAngleRad);
			ConeHalfAngleRad = FMath::Sqrt(1.0 / ConeHalfAngleRad);

			// clamp to make a cone instead of a sphere
			Phi = FMath::Fmod(Phi, ConeHalfAngleRad);

			// get axes we need to rotate around
			const FMatrix DirMat = FRotationMatrix(Dir.Rotation());
			// note the axis translation, since we want the variation to be around X
			const FVector DirZ = DirMat.GetUnitAxis(EAxis::X);
			const FVector DirY = DirMat.GetUnitAxis(EAxis::Y);

			FVector Result = Dir.RotateAngleAxis(FMath::RadiansToDegrees(Phi), DirY);
			Result = Result.RotateAngleAxis(FMath::RadiansToDegrees(Theta), DirZ);

			// ensure it's a unit vector (might not have been passed in that way)
			Result = Result.GetSafeNormal();

			return Result;
		}

		return Dir.GetSafeNormal();
	}

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
	bool RandomFromFractionChecked(const int32 Numerator, const int32 Denominator) const
	{
		check(Numerator >= 0)
		check(Numerator <= Denominator)
		check(Denominator > 0)

		return RandomFromFraction(Numerator, Denominator);
	}

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
		return Ar << MersenneTwister.InitialSeed << MersenneTwister.Engine << MersenneTwister.bIsInitialized;
	}

	bool Serialize(FArchive& Ar)
	{
		Ar << *this;
		return true;
	}

	friend void operator<<(FStructuredArchive::FSlot Slot, FMersenneTwister& MersenneTwister)
	{
		Slot << MersenneTwister.InitialSeed;
		Slot << MersenneTwister.Engine;
		Slot << MersenneTwister.bIsInitialized;
	}

	bool Serialize(const FStructuredArchive::FSlot Slot)
	{
		Slot << *this;
		return true;
	}
};

const FName RandomAccessToEmptyArrayWarning = FName("RandomAccessToEmptyArrayWarning");

UCLASS()
class APOLOGUECORE_API UMersenneTwisterLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	static bool EnsureInitialized(const FMersenneTwister& MersenneTwister)
	{
		if (MersenneTwister.IsInitialized())
		{
			return true;
		}

		ThrowBlueprintException();
		return false;
	}

	static void ThrowBlueprintException()
	{
		const UObject* ActiveObject = nullptr;
		const FBlueprintExceptionInfo Info(EBlueprintExceptionType::NonFatalError);
		FFrame& StackFrame = *(FBlueprintContextTracker::Get().GetCurrentScriptStackWritable().Last());
		FBlueprintCoreDelegates::ThrowScriptException(ActiveObject, StackFrame, Info);
	}

public:
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister")
	static FORCEINLINE bool GetInitialSeed(UPARAM(Ref) const FMersenneTwister& MersenneTwister, int64& InitialSeed)
	{
		if (!MersenneTwister.IsInitialized())
			return false;

		InitialSeed = MersenneTwister.GetInitialSeed();
		return true;
	}

	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister")
	static FORCEINLINE bool GetState(UPARAM(Ref) const FMersenneTwister& MersenneTwister, TArray<int64>& State, int32& Index)
	{
		if (!MersenneTwister.IsInitialized())
			return false;

		MersenneTwister.GetState(reinterpret_cast<TArray<FMersenneTwister::FEngineType::result_type>&>(State));
		Index = MersenneTwister.GetStateIndex();
		return true;
	}

	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister")
	static FORCEINLINE bool IsInitialized(UPARAM(Ref) const FMersenneTwister& MersenneTwister)
	{
		return MersenneTwister.IsInitialized();
	}

	/**
	 * Obtains a random bool.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Bool")
	static FORCEINLINE bool RandBool(UPARAM(Ref) const FMersenneTwister& MersenneTwister)
	{
		if (EnsureInitialized(MersenneTwister))
		{
			return MersenneTwister.RandHelper(2) == 1;
		}

		return false;
	}

	/**
	 * Obtains a random value in [Min, Max].
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Range (Integer)")
	static FORCEINLINE int32 RandRange_Int32(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const int32 Min, const int32 Max)
	{
		if (EnsureInitialized(MersenneTwister))
		{
			return MersenneTwister.RandomRange(Min, Max);
		}

		return 0;
	}

	/**
	 * Obtains a random value in [Min, Max].
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Range (Integer64)")
	static FORCEINLINE int64 RandRange_Int64(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const int64 Min, const int64 Max)
	{
		if (EnsureInitialized(MersenneTwister))
		{
			return MersenneTwister.RandomRange(Min, Max);
		}

		return 0;
	}

	/**
	 * Obtains a random value in [Min, Max).
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Range (Float)")
	static FORCEINLINE double RandRange_Double(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const double Min, const double Max)
	{
		if (EnsureInitialized(MersenneTwister))
		{
			return MersenneTwister.RandomRange(Min, Max);
		}

		return 0.0;
	}

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
	static FORCEINLINE double RandomFraction(UPARAM(Ref) const FMersenneTwister& MersenneTwister)
	{
		if (EnsureInitialized(MersenneTwister))
		{
			return MersenneTwister.GetFraction<double>();
		}

		return 0.0;
	}

	/**
	 * Returns a random vector of unit size.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister")
	static FORCEINLINE FVector RandomUnitVector(UPARAM(Ref) const FMersenneTwister& MersenneTwister)
	{
		if (EnsureInitialized(MersenneTwister))
		{
			return MersenneTwister.GetUnitVector();
		}

		return FVector::ZeroVector;
	}

	/**
	 * Returns a random 2D point in a unit circle.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Point in Unit Circle")
	static FORCEINLINE FVector2D RandomPointInUnitCircle(UPARAM(Ref) const FMersenneTwister& MersenneTwister)
	{
		if (EnsureInitialized(MersenneTwister))
		{
			return MersenneTwister.GetPointInUnitCircle();
		}

		return FVector2D::ZeroVector;
	}

	/**
	 * Returns a random 3D point in a unit sphere.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Point in Unit Sphere")
	static FORCEINLINE FVector RandomPointInUnitSphere(UPARAM(Ref) const FMersenneTwister& MersenneTwister)
	{
		if (EnsureInitialized(MersenneTwister))
		{
			return MersenneTwister.GetPointInUnitSphere();
		}

		return FVector::ZeroVector;
	}

	/**
	 * Returns a random point in a bounding box.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Point in Bounding Box")
	static FORCEINLINE FVector RandomPointInBoundingBox(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const FVector& Center, const FVector& HalfSize)
	{
		if (EnsureInitialized(MersenneTwister))
		{
			return MersenneTwister.GetPointInBoundingBox(Center, HalfSize);
		}

		return FVector::ZeroVector;
	}

	/**
	 * Returns a random point in a box.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Point in Box")
	static FORCEINLINE FVector RandomPointInBox(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const FBox& Box)
	{
		if (EnsureInitialized(MersenneTwister))
		{
			return MersenneTwister.GetPointInBox(Box);
		}

		return FVector::ZeroVector;
	}

	/**
	 * Returns a random unit vector, uniformly distributed, within the specified cone.
	 *
	 * @param MersenneTwister MersenneTwister.
	 * @param Direction The center direction of the cone.
	 * @param HalfAngle Half-angle of cone, in degrees.
	 * @return Normalized vector within the specified cone.
	 */
	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random Cone")
	static FORCEINLINE FVector RandomCone(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const FVector& Direction, const double HalfAngle)
	{
		if (EnsureInitialized(MersenneTwister))
		{
			return MersenneTwister.GetCone(Direction, FMath::DegreesToRadians(HalfAngle));
		}

		return FVector::ZeroVector;
	}

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
	static FORCEINLINE FVector GetConeWithVertical(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const FVector& Direction, const double HalfAngle,
	                                               const double VerticalHalfAngle)
	{
		if (EnsureInitialized(MersenneTwister))
		{
			return MersenneTwister.GetCone(Direction, FMath::DegreesToRadians(HalfAngle), FMath::DegreesToRadians(VerticalHalfAngle));
		}

		return FVector::ZeroVector;
	}

	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random from Fraction (Integer)")
	static FORCEINLINE int32 RandomFromFraction_Int32(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const int32 Numerator, const int32 Denominator)
	{
		if (EnsureInitialized(MersenneTwister))
		{
			return MersenneTwister.RandomFromFraction(Numerator, Denominator);
		}

		return 0;
	}

	UFUNCTION(BlueprintCallable, Category="Apologue|Random|Mersenne Twister", DisplayName="Random from Fraction (Integer64)")
	static FORCEINLINE int64 RandomFromFraction_Int64(UPARAM(Ref) const FMersenneTwister& MersenneTwister, const int64 Numerator, const int64 Denominator)
	{
		if (EnsureInitialized(MersenneTwister))
		{
			return MersenneTwister.RandomFromFraction(Numerator, Denominator);
		}

		return 0;
	}

private:
	// ReSharper disable CppParameterMayBeConstPtrOrRef
	static FORCEINLINE void GenericRandArray(void* TargetArray, const FArrayProperty* ArrayProperty, FMersenneTwister* MersenneTwister, void* OutElement, int32* OutIndex)
	// ReSharper restore CppParameterMayBeConstPtrOrRef
	{
		*OutIndex = INDEX_NONE;
		if (!TargetArray || !MersenneTwister)
		{
			return;
		}

		FScriptArrayHelper ArrayHelper(ArrayProperty, TargetArray);
		const FProperty* InnerProp = ArrayProperty->Inner;

		if (ArrayHelper.Num() > 0)
		{
			const int32 Index = MersenneTwister->RandHelper(ArrayHelper.Num());

			InnerProp->CopySingleValueToScriptVM(OutElement, ArrayHelper.GetRawPtr(Index));
			*OutIndex = Index;
			return;
		}

		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Attempted to access random index from empty array!")), ELogVerbosity::Warning, RandomAccessToEmptyArrayWarning);
		InnerProp->InitializeValue(OutElement);
	}

	// ReSharper disable CppParameterMayBeConstPtrOrRef
	static FORCEINLINE void GenericShuffleArray(void* TargetArray, const FArrayProperty* ArrayProperty, FMersenneTwister* MersenneTwister)
	// ReSharper restore CppParameterMayBeConstPtrOrRef
	{
		if (!TargetArray || !MersenneTwister)
		{
			return;
		}

		FScriptArrayHelper ArrayHelper(ArrayProperty, TargetArray);
		const int32 LastIndex = ArrayHelper.Num() - 1;
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			const int32 Index = MersenneTwister->RandomRange(i, LastIndex);
			if (i != Index)
			{
				ArrayHelper.SwapValues(i, Index);
			}
		}
	}

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
