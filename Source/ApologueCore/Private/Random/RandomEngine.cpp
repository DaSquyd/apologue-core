#include "Random/RandomEngine.h"

#include "Random/RandomUtil.h"

void FRandomEngine::Initialize()
{
	// hardware randomization
	// TODO: Is this the best way to handle this across all devices?
	Initialize(FRandomUtil::GetSeedFromHardware());
}

void FRandomEngine::Initialize(const uint64 Seed)
{
	InitialSeed = Seed;
	Initialize_Implementation(Seed);
	bIsInitialized = true;
}

void FRandomEngine::Initialize(const FString& Seed)
{
	Initialize(FRandomUtil::GetSeedFromString(Seed));
}

void FRandomEngine::Initialize_Implementation(const uint64 Seed)
{
	unimplemented()
}

uint32 FRandomEngine::Random()
{
	unimplemented()
	return 0;
}

FVector FRandomEngine::GetUnitVector()
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

FVector2D FRandomEngine::GetPointInUnitCircle()
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

FVector FRandomEngine::GetPointInUnitSphere()
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

FVector FRandomEngine::GetPointInBoundingBox(const FVector& Center, const FVector& HalfSize)
{
	const FVector BoxMin = Center - HalfSize;
	const FVector BoxMax = Center + HalfSize;
	return GetPointInBox(FBox(BoxMin, BoxMax));
}

FVector FRandomEngine::GetPointInBox(const FBox& Box)
{
	return FVector(RandomRange(Box.Min.X, Box.Max.X),
				   RandomRange(Box.Min.Y, Box.Max.Y),
				   RandomRange(Box.Min.Z, Box.Max.Z));
}

FVector FRandomEngine::GetCone(const FVector& Dir, const double ConeHalfAngleRad)
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

FVector FRandomEngine::GetCone(const FVector& Dir, const double HorizontalConeHalfAngleRad, const double VerticalConeHalfAngleRad)
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

void FRandomEngine::Discard(const int32 Count)
{
	unimplemented()
}
