#include "Random/MersenneTwisterHandler.h"
#if WITH_TESTS

#include "Random/MersenneTwister.h"

#include "Tests/TestHarnessAdapter.h"

TEST_CASE_NAMED(FMersenneTwisterTest, "ApologueCore::MersenneTwister", "[Apologue][ApologueCore][MersenneTwister]")
{
	SECTION("Consistent Seeding")
	{
		constexpr uint64 Seed = 0xDEADBEEF;

		FMersenneTwister TwisterA;
		TwisterA.Initialize(Seed);

		FMersenneTwister TwisterB;
		TwisterB.Initialize(Seed);

		CHECK(TwisterA.Random() == TwisterB.Random());
		CHECK(TwisterA.Random() == TwisterB.Random());
		CHECK(TwisterA.Random() == TwisterB.Random());
		CHECK(TwisterA.Random() == TwisterB.Random());
	}

	SECTION("Reset")
	{
		constexpr uint64 Seed = 0xDEADBEEF;

		FMersenneTwister TwisterA;
		TwisterA.Initialize(Seed);
		TwisterA.Random();
		const int32 ValueAForward = TwisterA.Random();

		FMersenneTwister TwisterB;
		TwisterB.Initialize(Seed);
		const int32 ValueB = TwisterB.Random();

		CHECK(ValueAForward != ValueB);

		TwisterA.Reset();
		const int32 ValueAInitial = TwisterA.Random();

		CHECK(ValueAInitial == ValueB);
	}

	SECTION("Serialization")
	{
		constexpr uint64 Seed = 0xDEADBEEF;

		FMersenneTwister SavedTwister;
		SavedTwister.Initialize(Seed);

		FMersenneTwister CompareTwister;
		CompareTwister.Initialize(Seed);

		// Shuffle state for entropy
		constexpr int32 Iterations = 128;
		SavedTwister.Discard(Iterations);
		CompareTwister.Discard(Iterations);

		TArray<uint8> Buffer;
		FMersenneTwister LoadedTwister;

		// Writer
		FMemoryWriter MemoryWriter(Buffer);
		SavedTwister.Serialize(MemoryWriter);

		// Reader
		FMemoryReader MemoryReader(Buffer);
		LoadedTwister.Serialize(MemoryReader);

		if (!LoadedTwister.IsInitialized())
		{
			CHECK(false)
			return;
		}

		CHECK(SavedTwister.Random() == CompareTwister.Random());
	}

	SECTION("UObject Serialization")
	{
		constexpr uint64 Seed = 0xDEADBEEF;

		UMersenneTwisterHandler* SavedTwister = NewObject<UMersenneTwisterHandler>();
		SavedTwister->Initialize(Seed);

		UMersenneTwisterHandler* CompareTwister = NewObject<UMersenneTwisterHandler>();
		CompareTwister->Initialize(Seed);

		// Shuffle state for entropy
		constexpr int32 Iterations = 128;
		SavedTwister->Discard(Iterations);
		CompareTwister->Discard(Iterations);

		TArray<uint8> Buffer;
		UMersenneTwisterHandler* LoadedTwister = NewObject<UMersenneTwisterHandler>();

		// Writer
		FMemoryWriter MemoryWriter(Buffer);
		SavedTwister->Serialize(MemoryWriter);

		// Reader
		FMemoryReader MemoryReader(Buffer);
		LoadedTwister->Serialize(MemoryReader);

		if (!LoadedTwister->IsInitialized())
		{
			CHECK(false)
			return;
		}

		uint32 Random1 = SavedTwister->Random();
		uint32 Random2 = CompareTwister->Random();

		CHECK(Random1 == Random2);
	}
}

#endif
