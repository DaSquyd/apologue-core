#include "Random/MersenneTwister.h"

#include "Internationalization/Regex.h"
#include "Random/RandomUtil.h"

#if DO_BLUEPRINT_GUARD
#include "Blueprint/BlueprintExceptionInfo.h"
#include "UObject/Script.h"
#endif // DO_BLUEPRINT_GUARD

#define LOCTEXT_NAMESPACE "MersenneTwister"

void FMersenneTwister::Initialize()
{
	// hardware randomization
	// TODO: Is this the best way to handle this across all devices?
	Initialize(FRandomUtil::GetSeedFromHardware());
}

void FMersenneTwister::Initialize(const uint64 Seed)
{
	InitialSeed = Seed;
	Engine.seed(Seed);
	bIsInitialized = true;
}

void FMersenneTwister::Initialize(const FString& Seed)
{
	Initialize(FRandomUtil::GetSeedFromString(Seed));
}

void FMersenneTwister::GetState(TArray<FEngineType::result_type>& Array) const
{
	constexpr int32 StateSize = 2 * FEngineType::state_size;
	Array.Reset(StateSize);

	for (int32 Index = 0; Index < StateSize; ++Index)
	{
		Array.Add(Engine._Ax[Index]);
	}
}

FVector FMersenneTwister::GetUnitVector() const
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

FVector2D FMersenneTwister::GetPointInUnitCircle() const
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

FVector FMersenneTwister::GetPointInUnitSphere() const
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

FVector FMersenneTwister::GetPointInBoundingBox(const FVector& Center, const FVector& HalfSize) const
{
	const FVector BoxMin = Center - HalfSize;
	const FVector BoxMax = Center + HalfSize;
	return GetPointInBox(FBox(BoxMin, BoxMax));
}

FVector FMersenneTwister::GetPointInBox(const FBox& Box) const
{
	return FVector(RandomRange(Box.Min.X, Box.Max.X),
				   RandomRange(Box.Min.Y, Box.Max.Y),
				   RandomRange(Box.Min.Z, Box.Max.Z));
}

FVector FMersenneTwister::GetCone(const FVector& Dir, const double ConeHalfAngleRad) const
{
	if (ConeHalfAngleRad <= 0.f)
	{
		return Dir.GetSafeNormal();
	}
	
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

FVector FMersenneTwister::GetCone(const FVector& Dir, const double HorizontalConeHalfAngleRad, const double VerticalConeHalfAngleRad) const
{
	if (VerticalConeHalfAngleRad <= 0.0 || HorizontalConeHalfAngleRad <= 0.0)
	{
		return Dir.GetSafeNormal();
	}
	
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

bool FMersenneTwister::RandomFromFractionChecked(const int32 Numerator, const int32 Denominator) const
{
	check(Numerator >= 0)
	check(Numerator <= Denominator)
	check(Denominator > 0)

	return RandomFromFraction(Numerator, Denominator);
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
	FFrame& StackFrame = *(FBlueprintContextTracker::Get().GetCurrentScriptStackWritable().Last());
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
		return MersenneTwister.RandHelper(2) == 1;
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
		const int32 Index = MersenneTwister->RandHelper(ArrayHelper.Num());

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
