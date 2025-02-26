#if WITH_TESTS

#include "Random/MersenneTwister.h"

#include "Containers/UnrealString.h"
#include "HAL/FileManager.h"
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

		CHECK(TwisterA.RandHelper(Rand) == TwisterB.RandHelper(Rand));
		CHECK(TwisterA.RandHelper(Rand) == TwisterB.RandHelper(Rand));
		CHECK(TwisterA.RandHelper(Rand) == TwisterB.RandHelper(Rand));
		CHECK(TwisterA.RandHelper(Rand) == TwisterB.RandHelper(Rand));
	}

	SECTION("Reset")
	{
		constexpr uint64 Seed = 0xDEADBEEF;
		constexpr uint64 Rand = -1;
		
		FMersenneTwister TwisterA;
		TwisterA.Initialize(Seed);
		TwisterA.RandHelper(Rand);
		const int32 ValueAForward = TwisterA.RandHelper(Rand);
		
		FMersenneTwister TwisterB;
		TwisterB.Initialize(Seed);
		const int32 ValueB = TwisterB.RandHelper(Rand);

		CHECK(ValueAForward != ValueB);

		TwisterA.Reset();
		const int32 ValueAInitial = TwisterA.RandHelper(Rand);

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
			SavedTwister.RandHelper(1);
			CompareTwister.RandHelper(1);
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
		
		CHECK(CompareTwister.RandHelper(Rand) == CompareTwister.RandHelper(Rand));
	}
}

#endif
