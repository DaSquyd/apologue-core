#if WITH_TESTS

#include "Random/LinearCongruentialGeneratorHandler.h"

#include "Tests/TestHarnessAdapter.h"

TEST_CASE_NAMED(FLinearCongruentialGeneratorTest, "ApologueCore::LinearCongruentialGenerator", "[Apologue][ApologueCore][LinearCongruentialGenerator]")
{
	SECTION("Consistent Seeding")
	{
		constexpr uint64 Seed = 0xDEADBEEF;
		
		FLinearCongruentialGenerator GeneratorA;
		GeneratorA.Initialize(Seed);
		
		FLinearCongruentialGenerator GeneratorB;
		GeneratorB.Initialize(Seed);

		CHECK(GeneratorA.Random() == GeneratorB.Random());
		CHECK(GeneratorA.Random() == GeneratorB.Random());
		CHECK(GeneratorA.Random() == GeneratorB.Random());
		CHECK(GeneratorA.Random() == GeneratorB.Random());
	}

	SECTION("Reset")
	{
		constexpr uint64 Seed = 0xDEADBEEF;
		
		FLinearCongruentialGenerator GeneratorA;
		GeneratorA.Initialize(Seed);
		GeneratorA.Random();
		const int32 ValueAForward = GeneratorA.Random();
		
		FLinearCongruentialGenerator GeneratorB;
		GeneratorB.Initialize(Seed);
		const int32 ValueB = GeneratorB.Random();

		CHECK(ValueAForward != ValueB);

		GeneratorA.Reset();
		const int32 ValueAInitial = GeneratorA.Random();

		CHECK(ValueAInitial == ValueB);
	}

	SECTION("Serialization")
	{
		constexpr uint64 Seed = 0xDEADBEEF;
		
		FLinearCongruentialGenerator SavedGenerator;
		SavedGenerator.Initialize(Seed);
		
		FLinearCongruentialGenerator CompareGenerator;
		CompareGenerator.Initialize(Seed);

		// Shuffle state for entropy
		constexpr int32 Iterations = 128;
		SavedGenerator.Discard(Iterations);
		CompareGenerator.Discard(Iterations);

		TArray<uint8> Buffer;
		FLinearCongruentialGenerator LoadedGenerator;
		
		// Writer
		FMemoryWriter MemoryWriter(Buffer);
		SavedGenerator.Serialize(MemoryWriter);

		// Reader
		FMemoryReader MemoryReader(Buffer);
		LoadedGenerator.Serialize(MemoryReader);

		if(!LoadedGenerator.IsInitialized())
		{
			CHECK(false)
			return;
		}
		
		CHECK(SavedGenerator.Random() == CompareGenerator.Random());
	}

	SECTION("UObject Serialization")
	{
		constexpr uint64 Seed = 0xDEADBEEF;
		
		ULinearCongruentialGeneratorHandler* SavedGenerator = NewObject<ULinearCongruentialGeneratorHandler>();
		SavedGenerator->Initialize(Seed);
		
		ULinearCongruentialGeneratorHandler* CompareGenerator = NewObject<ULinearCongruentialGeneratorHandler>();
		CompareGenerator->Initialize(Seed);

		// Shuffle state for entropy
		constexpr int32 Iterations = 128;
		SavedGenerator->Discard(Iterations);
		CompareGenerator->Discard(Iterations);

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

		uint32 Random1 = SavedGenerator->Random();
		uint32 Random2 = CompareGenerator->Random();
		
		CHECK(Random1 == Random2);
	}
}

#endif
