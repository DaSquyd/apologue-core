// Fill out your copyright notice in the Description page of Project Settings.


#include "Random/RandomEngineFunctionLibrary.h"

#if DO_BLUEPRINT_GUARD
#include "Blueprint/BlueprintExceptionInfo.h"
#include "UObject/Script.h"
#endif // DO_BLUEPRINT_GUARD

#define LOCTEXT_NAMESPACE "RandomEngine"

bool URandomEngineFunctionLibrary::EnsureInitialized(const FRandomEngine& Engine)
{
	if (Engine.IsInitialized())
	{
		return true;
	}

#if DO_BLUEPRINT_GUARD
	ThrowBlueprintException(LOCTEXT("NotInitializedException", "Mersenne Twister was not initialized"));
#endif // DO_BLUEPRINT_GUARD
	
	return false;
}

#if DO_BLUEPRINT_GUARD
void URandomEngineFunctionLibrary::ThrowBlueprintException(const FText& Text)
{
	const UObject* ActiveObject = nullptr;
	const FBlueprintExceptionInfo Info(EBlueprintExceptionType::NonFatalError, Text);
	FFrame& StackFrame = *FBlueprintContextTracker::Get().GetCurrentScriptStackWritable().Last();
	FBlueprintCoreDelegates::ThrowScriptException(ActiveObject, StackFrame, Info);
}
#endif // DO_BLUEPRINT_GUARD

bool URandomEngineFunctionLibrary::GetInitialSeed(const FRandomEngine& Engine, int64& InitialSeed)
{
	if (!Engine.IsInitialized())
		return false;

	InitialSeed = Engine.GetInitialSeed();
	return true;
}

bool URandomEngineFunctionLibrary::RandBool(FRandomEngine& Engine)
{
	if (EnsureInitialized(Engine))
	{
		return Engine.Random(2) == 1;
	}

	return false;
}

int32 URandomEngineFunctionLibrary::RandRange_Int32(FRandomEngine& Engine, const int32 Min, const int32 Max)
{
	if (EnsureInitialized(Engine))
	{
		return Engine.RandomRange<int32>(Min, Max);
	}

	return 0;
}

int64 URandomEngineFunctionLibrary::RandRange_Int64(FRandomEngine& Engine, const int64 Min, const int64 Max)
{
	if (EnsureInitialized(Engine))
	{
		return Engine.RandomRange(Min, Max);
	}

	return 0;
}

double URandomEngineFunctionLibrary::RandRange_Double(FRandomEngine& Engine, const double Min, const double Max)
{
	if (EnsureInitialized(Engine))
	{
		return Engine.RandomRange(Min, Max);
	}

	return 0.0;
}

double URandomEngineFunctionLibrary::RandomFraction(FRandomEngine& Engine)
{
	if (EnsureInitialized(Engine))
	{
		return Engine.GetFraction<double>();
	}

	return 0.0;
}

FVector URandomEngineFunctionLibrary::RandomUnitVector(FRandomEngine& Engine)
{
	if (EnsureInitialized(Engine))
	{
		return Engine.GetUnitVector();
	}

	return FVector::ZeroVector;
}

FVector2D URandomEngineFunctionLibrary::RandomPointInUnitCircle(FRandomEngine& Engine)
{
	if (EnsureInitialized(Engine))
	{
		return Engine.GetPointInUnitCircle();
	}

	return FVector2D::ZeroVector;
}

FVector URandomEngineFunctionLibrary::RandomPointInUnitSphere(FRandomEngine& Engine)
{
	if (EnsureInitialized(Engine))
	{
		return Engine.GetPointInUnitSphere();
	}

	return FVector::ZeroVector;
}

FVector URandomEngineFunctionLibrary::RandomPointInBoundingBox(FRandomEngine& Engine, const FVector& Center, const FVector& HalfSize)
{
	if (EnsureInitialized(Engine))
	{
		return Engine.GetPointInBoundingBox(Center, HalfSize);
	}

	return FVector::ZeroVector;
}

FVector URandomEngineFunctionLibrary::RandomPointInBox(FRandomEngine& Engine, const FBox& Box)
{
	if (EnsureInitialized(Engine))
	{
		return Engine.GetPointInBox(Box);
	}

	return FVector::ZeroVector;
}

FVector URandomEngineFunctionLibrary::RandomCone(FRandomEngine& Engine, const FVector& Direction, const double HalfAngle)
{
	if (EnsureInitialized(Engine))
	{
		return Engine.GetCone(Direction, FMath::DegreesToRadians(HalfAngle));
	}

	return FVector::ZeroVector;
}

FVector URandomEngineFunctionLibrary::GetConeWithVertical(FRandomEngine& Engine, const FVector& Direction, const double HalfAngle, const double VerticalHalfAngle)
{
	if (EnsureInitialized(Engine))
	{
		return Engine.GetCone(Direction, FMath::DegreesToRadians(HalfAngle), FMath::DegreesToRadians(VerticalHalfAngle));
	}

	return FVector::ZeroVector;
}

int32 URandomEngineFunctionLibrary::RandomFromFraction_Int32(FRandomEngine& Engine, const int32 Numerator, const int32 Denominator)
{
	if (EnsureInitialized(Engine))
	{
		return Engine.RandomFromFraction(Numerator, Denominator);
	}

	return 0;
}

// ReSharper disable CppParameterMayBeConstPtrOrRef
void URandomEngineFunctionLibrary::GenericRandArray(void* TargetArray, const FArrayProperty* ArrayProperty, FRandomEngine* Engine, void* OutElement, int32* OutIndex)
// ReSharper restore CppParameterMayBeConstPtrOrRef
{
	*OutIndex = INDEX_NONE;
	if (!TargetArray || !Engine)
	{
		return;
	}

	FScriptArrayHelper ArrayHelper(ArrayProperty, TargetArray);
	const FProperty* InnerProp = ArrayProperty->Inner;

	if (ArrayHelper.Num() > 0)
	{
		const int32 Index = Engine->Random(ArrayHelper.Num());

		InnerProp->CopySingleValueToScriptVM(OutElement, ArrayHelper.GetRawPtr(Index));
		*OutIndex = Index;
		return;
	}

	FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Attempted to access random index from empty array!")), ELogVerbosity::Warning, FName("RandomAccessToEmptyArrayWarning"));
	InnerProp->InitializeValue(OutElement);
}

// ReSharper disable CppParameterMayBeConstPtrOrRef
void URandomEngineFunctionLibrary::GenericShuffleArray(void* TargetArray, const FArrayProperty* ArrayProperty, FRandomEngine* Engine)
// ReSharper restore CppParameterMayBeConstPtrOrRef
{
	if (!TargetArray || !Engine)
	{
		return;
	}

	FScriptArrayHelper ArrayHelper(ArrayProperty, TargetArray);
	const int32 LastIndex = ArrayHelper.Num() - 1;
	for (int32 i = 0; i <= LastIndex; ++i)
	{
		const int32 Index = Engine->RandomRange(i, LastIndex);
		if (i != Index)
		{
			ArrayHelper.SwapValues(i, Index);
		}
	}
}