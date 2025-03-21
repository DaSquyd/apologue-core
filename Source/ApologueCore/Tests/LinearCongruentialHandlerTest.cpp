#if WITH_TESTS

#include "Random/LinearCongruentialGeneratorHandler.h"

#include "Tests/TestHarnessAdapter.h"

TEST_CASE_NAMED(FMersenneTwisterTest, "ApologueCore::MersenneTwister", "[Apologue][ApologueCore][MersenneTwister]")
{
	SECTION("Consistent Seeding")
	{
		constexpr uint64 Seed = 0xDEADBEEF;
		constexpr uint64 Rand = -1;
		
		ULinearCongruentialGeneratorHandler* GeneratorA = NewObject<ULinearCongruentialGeneratorHandler>();
		GeneratorA->Initialize(Seed);
		
		ULinearCongruentialGeneratorHandler* GeneratorB = NewObject<ULinearCongruentialGeneratorHandler>();
		GeneratorB->Initialize(Seed);

		CHECK(GeneratorA->Random(Rand) == GeneratorB->Random(Rand));
		CHECK(GeneratorA->Random(Rand) == GeneratorB->Random(Rand));
		CHECK(GeneratorA->Random(Rand) == GeneratorB->Random(Rand));
		CHECK(GeneratorA->Random(Rand) == GeneratorB->Random(Rand));
	}

	SECTION("Reset")
	{
		constexpr uint64 Seed = 0xDEADBEEF;
		constexpr uint64 Rand = -1;
		
		ULinearCongruentialGeneratorHandler* GeneratorA = NewObject<ULinearCongruentialGeneratorHandler>();
		GeneratorA->Initialize(Seed);
		GeneratorA->Random(Rand);
		const int32 ValueAForward = GeneratorA->Random(Rand);
		
		ULinearCongruentialGeneratorHandler* GeneratorB = NewObject<ULinearCongruentialGeneratorHandler>();
		GeneratorB->Initialize(Seed);
		const int32 ValueB = GeneratorB->Random(Rand);

		CHECK(ValueAForward != ValueB);

		GeneratorA->Reset();
		const int32 ValueAInitial = GeneratorA->Random(Rand);

		CHECK(ValueAInitial == ValueB);
	}

	SECTION("Serialization")
	{
		constexpr uint64 Seed = 0xDEADBEEF;
		constexpr uint64 Rand = -1;
		
		ULinearCongruentialGeneratorHandler* SavedGenerator = NewObject<ULinearCongruentialGeneratorHandler>();
		SavedGenerator->Initialize(Seed);
		
		ULinearCongruentialGeneratorHandler* CompareGenerator = NewObject<ULinearCongruentialGeneratorHandler>();
		CompareGenerator->Initialize(Seed);

		// Shuffle state for entropy
		constexpr int32 Iterations = 128;
		for (int32 i = 0; i < Iterations; i++)
		{
			SavedGenerator->Random(1);
			SavedGenerator->Random(1);
		}

		TArray<uint8> Buffer;
		ULinearCongruentialGeneratorHandler* LoadedGenerator = NewObject<ULinearCongruentialGeneratorHandler>();
		
		// Writer
		FMemoryWriter MemoryWriter(Buffer);
		SavedGenerator->Serialize(MemoryWriter);

		// Reader
		FMemoryReader MemoryReader(Buffer);
		LoadedGenerator->Serialize(MemoryReader);

		if(!LoadedGenerator->IsInitialized())
		{
			CHECK(false)
			return;
		}
		
		CHECK(LoadedGenerator->Random(Rand) == CompareGenerator->Random(Rand));
	}
}

#endif
