// ReSharper disable CppRedundantCastExpression

#include "Random/MersenneTwister.h"

#include "Internationalization/Regex.h"
#include "Random/RandomUtil.h"

#if DO_BLUEPRINT_GUARD
#include "Blueprint/BlueprintExceptionInfo.h"
#include "UObject/Script.h"
#endif // DO_BLUEPRINT_GUARD

#define LOCTEXT_NAMESPACE "MersenneTwister"

void FMersenneTwister::Initialize_Implementation(const uint64 Seed)
{
	Engine.seed(Seed);
}

void FMersenneTwister::GetState(TArray<FEngineType::result_type>& Array) const
{
	constexpr int32 StateSize = 2 * FEngineType::state_size;
	Array.Reset(StateSize);

	for (int32 Index = 0; Index < StateSize; ++Index)
	{
		const FEngineType::result_type* State = reinterpret_cast<FEngineType::_Mybase*>(&Engine)->_Ax;
		Array.Add(State[Index]);
	}
}

FVector FMersenneTwister::GetCone(const FVector& Dir, const double HorizontalConeHalfAngleRad, const double VerticalConeHalfAngleRad) const
{
	
}

bool FMersenneTwister::RandomFromFractionChecked(const int32 Numerator, const int32 Denominator) const
{
	check(Numerator >= 0)
	check(Numerator <= Denominator)
	check(Denominator > 0)

	return RandomFromFraction(Numerator, Denominator);
}

void FMersenneTwister::Discard(const int32 Count) const
{
	Engine.discard(Count);
}

bool UMersenneTwisterLibrary::EnsureInitialized(const FMersenneTwister& MersenneTwister)
{
	if (MersenneTwister.IsInitialized())
	{
		return true;
	}

#if DO_BLUEPRINT_GUARD
	ThrowBlueprintException(LOCTEXT("NotInitializedException", "Mersenne Twister was not initialized"));
#endif // DO_BLUEPRINT_GUARD
	
	return false;
}

#if DO_BLUEPRINT_GUARD
void UMersenneTwisterLibrary::ThrowBlueprintException(const FText& Text)
{
	const UObject* ActiveObject = nullptr;
	const FBlueprintExceptionInfo Info(EBlueprintExceptionType::NonFatalError, Text);
	FFrame& StackFrame = *FBlueprintContextTracker::Get().GetCurrentScriptStackWritable().Last();
	FBlueprintCoreDelegates::ThrowScriptException(ActiveObject, StackFrame, Info);
}
#endif // DO_BLUEPRINT_GUARD

bool UMersenneTwisterLibrary::GetInitialSeed(const FMersenneTwister& MersenneTwister, int64& InitialSeed)
{
	if (!MersenneTwister.IsInitialized())
		return false;

	InitialSeed = MersenneTwister.GetInitialSeed();
	return true;
}

bool UMersenneTwisterLibrary::GetState(const FMersenneTwister& MersenneTwister, TArray<int64>& State, int32& Index)
{
	if (!MersenneTwister.IsInitialized())
		return false;

	MersenneTwister.GetState(reinterpret_cast<TArray<FMersenneTwister::FEngineType::result_type>&>(State));
	Index = MersenneTwister.GetStateIndex();
	return true;
}

bool UMersenneTwisterLibrary::RandBool(const FMersenneTwister& MersenneTwister)
{
	if (EnsureInitialized(MersenneTwister))
	{
		return MersenneTwister.Random(2) == 1;
	}

	return false;
}

int32 UMersenneTwisterLibrary::RandRange_Int32(const FMersenneTwister& MersenneTwister, const int32 Min, const int32 Max)
{
	if (EnsureInitialized(MersenneTwister))
	{
		return MersenneTwister.RandomRange(Min, Max);
	}

	return 0;
}

int64 UMersenneTwisterLibrary::RandRange_Int64(const FMersenneTwister& MersenneTwister, const int64 Min, const int64 Max)
{
	if (EnsureInitialized(MersenneTwister))
	{
		return MersenneTwister.RandomRange(Min, Max);
	}

	return 0;
}

double UMersenneTwisterLibrary::RandRange_Double(const FMersenneTwister& MersenneTwister, const double Min, const double Max)
{
	if (EnsureInitialized(MersenneTwister))
	{
		return MersenneTwister.RandomRange(Min, Max);
	}

	return 0.0;
}

double UMersenneTwisterLibrary::RandomFraction(const FMersenneTwister& MersenneTwister)
{
	if (EnsureInitialized(MersenneTwister))
	{
		return MersenneTwister.GetFraction<double>();
	}

	return 0.0;
}

FVector UMersenneTwisterLibrary::RandomUnitVector(const FMersenneTwister& MersenneTwister)
{
	if (EnsureInitialized(MersenneTwister))
	{
		return MersenneTwister.GetUnitVector();
	}

	return FVector::ZeroVector;
}

FVector2D UMersenneTwisterLibrary::RandomPointInUnitCircle(const FMersenneTwister& MersenneTwister)
{
	if (EnsureInitialized(MersenneTwister))
	{
		return MersenneTwister.GetPointInUnitCircle();
	}

	return FVector2D::ZeroVector;
}

FVector UMersenneTwisterLibrary::RandomPointInUnitSphere(const FMersenneTwister& MersenneTwister)
{
	if (EnsureInitialized(MersenneTwister))
	{
		return MersenneTwister.GetPointInUnitSphere();
	}

	return FVector::ZeroVector;
}

FVector UMersenneTwisterLibrary::RandomPointInBoundingBox(const FMersenneTwister& MersenneTwister, const FVector& Center, const FVector& HalfSize)
{
	if (EnsureInitialized(MersenneTwister))
	{
		return MersenneTwister.GetPointInBoundingBox(Center, HalfSize);
	}

	return FVector::ZeroVector;
}

FVector UMersenneTwisterLibrary::RandomPointInBox(const FMersenneTwister& MersenneTwister, const FBox& Box)
{
	if (EnsureInitialized(MersenneTwister))
	{
		return MersenneTwister.GetPointInBox(Box);
	}

	return FVector::ZeroVector;
}

FVector UMersenneTwisterLibrary::RandomCone(const FMersenneTwister& MersenneTwister, const FVector& Direction, const double HalfAngle)
{
	if (EnsureInitialized(MersenneTwister))
	{
		return MersenneTwister.GetCone(Direction, FMath::DegreesToRadians(HalfAngle));
	}

	return FVector::ZeroVector;
}

FVector UMersenneTwisterLibrary::GetConeWithVertical(const FMersenneTwister& MersenneTwister, const FVector& Direction, const double HalfAngle, const double VerticalHalfAngle)
{
	if (EnsureInitialized(MersenneTwister))
	{
		return MersenneTwister.GetCone(Direction, FMath::DegreesToRadians(HalfAngle), FMath::DegreesToRadians(VerticalHalfAngle));
	}

	return FVector::ZeroVector;
}

int32 UMersenneTwisterLibrary::RandomFromFraction_Int32(const FMersenneTwister& MersenneTwister, const int32 Numerator, const int32 Denominator)
{
	if (EnsureInitialized(MersenneTwister))
	{
		return MersenneTwister.RandomFromFraction(Numerator, Denominator);
	}

	return 0;
}

int64 UMersenneTwisterLibrary::RandomFromFraction_Int64(const FMersenneTwister& MersenneTwister, const int64 Numerator, const int64 Denominator)
{
	if (EnsureInitialized(MersenneTwister))
	{
		return MersenneTwister.RandomFromFraction(Numerator, Denominator);
	}

	return 0;
}

// ReSharper disable CppParameterMayBeConstPtrOrRef
void UMersenneTwisterLibrary::GenericRandArray(void* TargetArray, const FArrayProperty* ArrayProperty, FMersenneTwister* MersenneTwister, void* OutElement, int32* OutIndex)
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
		const int32 Index = MersenneTwister->Random(ArrayHelper.Num());

		InnerProp->CopySingleValueToScriptVM(OutElement, ArrayHelper.GetRawPtr(Index));
		*OutIndex = Index;
		return;
	}

	FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Attempted to access random index from empty array!")), ELogVerbosity::Warning, RandomAccessToEmptyArrayWarning);
	InnerProp->InitializeValue(OutElement);
}

// ReSharper disable CppParameterMayBeConstPtrOrRef
void UMersenneTwisterLibrary::GenericShuffleArray(void* TargetArray, const FArrayProperty* ArrayProperty, FMersenneTwister* MersenneTwister)
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
