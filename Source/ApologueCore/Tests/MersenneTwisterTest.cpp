#if WITH_TESTS

#include "Random/MersenneTwister.h"

#include "Tests/TestHarnessAdapter.h"

TEST_CASE_NAMED(FMersenneTwisterTest, "ApologueCore::MersenneTwister", "[Apologue][ApologueCore][MersenneTwister]")
{
	SECTION("Consistent Seeding")
	{
		constexpr uint64 Seed = 0xDEADBEEF;
		constexpr uint64 Rand = -1;
		
		FMersenneTwister TwisterA;
		TwisterA.Initialize(Seed);
		
		FMersenneTwister TwisterB;
		TwisterB.Initialize(Seed);

		CHECK(TwisterA.Random(Rand) == TwisterB.Random(Rand));
		CHECK(TwisterA.Random(Rand) == TwisterB.Random(Rand));
		CHECK(TwisterA.Random(Rand) == TwisterB.Random(Rand));
		CHECK(TwisterA.Random(Rand) == TwisterB.Random(Rand));
	}

	SECTION("Reset")
	{
		constexpr uint64 Seed = 0xDEADBEEF;
		constexpr uint64 Rand = -1;
		
		FMersenneTwister TwisterA;
		TwisterA.Initialize(Seed);
		TwisterA.Random(Rand);
		const int32 ValueAForward = TwisterA.Random(Rand);
		
		FMersenneTwister TwisterB;
		TwisterB.Initialize(Seed);
		const int32 ValueB = TwisterB.Random(Rand);

		CHECK(ValueAForward != ValueB);

		TwisterA.Reset();
		const int32 ValueAInitial = TwisterA.Random(Rand);

		CHECK(ValueAInitial == ValueB);
	}

	SECTION("Serialization")
	{
		constexpr uint64 Seed = 0xDEADBEEF;
		constexpr uint64 Rand = -1;
		
		FMersenneTwister SavedTwister;
		SavedTwister.Initialize(Seed);
		
		FMersenneTwister CompareTwister;
		CompareTwister.Initialize(Seed);

		// Shuffle state for entropy
		constexpr int32 Iterations = 128;
		for (int32 i = 0; i < Iterations; i++)
		{
			SavedTwister.Random(1);
			CompareTwister.Random(1);
		}

		TArray<uint8> Buffer;
		FMersenneTwister LoadedTwister;
		
		// Writer
		FMemoryWriter MemoryWriter(Buffer);
		MemoryWriter << SavedTwister;

		// Reader
		FMemoryReader MemoryReader(Buffer);
		MemoryReader << LoadedTwister;

		if(!LoadedTwister.IsInitialized())
		{
			CHECK(false)
			return;
		}
		
		CHECK(SavedTwister.Random(Rand) == CompareTwister.Random(Rand));
	}
}

#endif
