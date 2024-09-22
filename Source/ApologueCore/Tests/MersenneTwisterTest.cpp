#if WITH_TESTS

#include "Random/MersenneTwister.h"

#include "Containers/UnrealString.h"
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
		
		FMersenneTwister TwisterA;
		TwisterA.Initialize(Seed);
		
		FMersenneTwister TwisterB;
		TwisterB.Initialize(Seed);

		constexpr int32 Iterations = 16;
		for (int32 i = 0; i < Iterations; i++)
		{
			TwisterA.RandHelper(1);
			TwisterB.RandHelper(1);
		}

		const FString Filename = TEXT("MersenneTwisterTest.bin");
		IFileManager& FileManager = IFileManager::Get();

		const std::unique_ptr<FArchive> WriteArchive = std::unique_ptr<FArchive>(FileManager.CreateFileWriter(*Filename));
		*WriteArchive << TwisterA;
		WriteArchive->Close();

		const std::unique_ptr<FArchive> ReadArchive = std::unique_ptr<FArchive>(FileManager.CreateFileReader(*Filename));
		FMersenneTwister TwisterC;
		*ReadArchive << TwisterC;
		ReadArchive->Close();

		FileManager.Delete(*Filename, true);

		if(!TwisterC.IsInitialized())
		{
			CHECK(false)
			return;
		}
		
		CHECK(TwisterB.RandHelper(Rand) == TwisterC.RandHelper(Rand));
	}
}

#endif
