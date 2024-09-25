// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ApologueStatTable.generated.h"

class UApologueStat;

// Struct for combining a StatType with an associated value
// Used for base stats, stat boosts, etc.
USTRUCT(BlueprintType)
struct FApologueStatTableEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UApologueStat> Stat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Value;

	FApologueStatTableEntry()
		: Value(0)
	{
	}

	FApologueStatTableEntry(const TSoftObjectPtr<UApologueStat>& Stat, const int32& Value)
		: Stat(Stat), Value(Value)
	{
	}

	// Serialization
	friend FArchive& operator<<(FArchive& Ar, FApologueStatTableEntry& StatTypeValue)
	{
		Ar << StatTypeValue.Stat;
		Ar << StatTypeValue.Value;
		return Ar;
	}

	bool Serialize(FArchive& Ar)
	{
		Ar << *this;
		return true;
	}

	friend void operator<<(FStructuredArchive::FSlot Slot, FApologueStatTableEntry& StatTypeValue)
	{
		Slot << StatTypeValue.Stat;
		Slot << StatTypeValue.Value;
	}

	bool Serialize(const FStructuredArchive::FSlot Slot)
	{
		Slot << *this;
		return true;
	}
};


USTRUCT(BlueprintType,
	meta=(HasNativeMake="/Script/ApologueCore.ApologueStatTableFunctionLibrary:Make", HasNativeBreak="/Script/ApologueCore.ApologueStatTableFunctionLibrary:Break"))
struct FApologueStatTable
{
	GENERATED_BODY()

	friend class UApologueStatTableFunctionLibrary;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TArray<FApologueStatTableEntry> Entries;

public:
	FApologueStatTable()
	{
	}

	explicit FApologueStatTable(const TArray<TSoftObjectPtr<UApologueStat>>& Stats, const int32 DefaultValue, const bool bKeepExistingValues = false)
	{
		Entries = TArray<FApologueStatTableEntry>();
		Entries.Reserve(Stats.Num());

		for (const TSoftObjectPtr<UApologueStat>& Stat : Stats)
		{
			if (!bKeepExistingValues)
			{
				Entries.Add(FApologueStatTableEntry(Stat, DefaultValue));
				continue;
			}

			int32 Value;
			if (!TryGet(Stat, Value))
			{
				Value = DefaultValue;
			}

			Entries.Add(FApologueStatTableEntry(Stat, Value));
		}
	}

	explicit FApologueStatTable(const TArray<FApologueStatTableEntry>& InEntries)
	{
		for (const FApologueStatTableEntry& Entry : InEntries)
		{
			if (ensure(!TrySet(Entry.Stat, Entry.Value)))
			{
				Entries.Add(Entry);
			}
		}
	}

	bool IsValid() const
	{
		return !Entries.IsEmpty();
	}

	void Reset()
	{
		Entries.Reset();
	}

	bool Has(const TSoftObjectPtr<UApologueStat>& Stat) const
	{
		return !!Entries.FindByPredicate([&Stat](const FApologueStatTableEntry& Entry)-> bool
		{
			return Entry.Stat == Stat;
		});
	}

	int32& Get(const TSoftObjectPtr<UApologueStat>& StatType)
	{
		FApologueStatTableEntry* EntryPtr = Entries.FindByPredicate([&StatType](const FApologueStatTableEntry& Entry)-> bool
		{
			return Entry.Stat == StatType;
		});

		check(EntryPtr);
		return EntryPtr->Value;
	}

	bool TryGet(const TSoftObjectPtr<UApologueStat>& Stat, int32& OutValue) const
	{
		if (const FApologueStatTableEntry* EntryPtr = Entries.FindByPredicate([&Stat](const FApologueStatTableEntry& Entry)-> bool
		{
			return Entry.Stat == Stat;
		}))
		{
			OutValue = EntryPtr->Value;
			return true;
		}

		OutValue = int32();
		return false;
	}

	bool TrySet(const TSoftObjectPtr<UApologueStat>& Stat, const int32& NewValue)
	{
		if (FApologueStatTableEntry* EntryPtr = Entries.FindByPredicate([&Stat](const FApologueStatTableEntry& Entry)-> bool
		{
			return Entry.Stat == Stat;
		}))
		{
			EntryPtr->Value = NewValue;
			return true;
		}

		return false;
	}

	void ForEach(const TFunctionRef<void (const TSoftObjectPtr<UApologueStat>& Stat, const int32& Value)>& Function) const
	{
		for (const FApologueStatTableEntry& Entry : Entries)
		{
			Function(Entry.Stat, Entry.Value);
		}
	}

	void ForEach(const TFunctionRef<void (const TSoftObjectPtr<UApologueStat>& StatType, int32& Value)>& Function)
	{
		for (FApologueStatTableEntry& Entry : Entries)
		{
			Function(Entry.Stat, Entry.Value);
		}
	}

	bool Validate() const
	{
		for (int32 I = 0; I < Num(); ++I)
		{
			for (int32 J = I + 1; J < Num(); ++J)
			{
				if (Entries[I].Stat == Entries[J].Stat)
				{
					return false;
				}
			}
		}

		return true;
	}

	void GetValues(TArray<int32>& OutValues)
	{
		OutValues.Reset(Num());
		for (FApologueStatTableEntry& Entry : Entries)
		{
			OutValues.Add(Entry.Value);
		}
	}

	int32 GetTotal() const
	{
		int32 Total = 0;
		for (const FApologueStatTableEntry& Entry : Entries)
		{
			Total += Entry.Value;
		}
		return Total;
	}

	int32 Num() const
	{
		return Entries.Num();
	}

	float GetAverage() const
	{
		float Total = 0;
		for (const FApologueStatTableEntry& Entry : Entries)
		{
			Total += Entry.Value;
		}
		return Total / Num();
	}

	int32 GetMaxValue() const
	{
		int32 Max = TNumericLimits<int32>::Min();
		for (int32 Index = 0; Index < Num(); ++Index)
		{
			if (Entries[Index].Value > Max)
			{
				Max = Entries[Index].Value;
			}
		}
		return Max;
	}

	int32 GetMinValue() const
	{
		int32 Min = TNumericLimits<int32>::Max();
		for (int32 Index = 0; Index < Num(); ++Index)
		{
			if (Entries[Index].Value < Min)
			{
				Min = Entries[Index].Value;
			}
		}
		return Min;
	}

	int32 GetMaxStats(TArray<TSoftObjectPtr<UApologueStat>>& OutMaxStats) const
	{
		int32 Max = TNumericLimits<int32>::Min();
		OutMaxStats.Reset();
		for (int32 Index = 0; Index < Num(); ++Index)
		{
			if (Entries[Index].Value < Max)
			{
				continue;
			}

			if (Entries[Index].Value > Max)
			{
				OutMaxStats.Reset();
			}

			Max = Entries[Index].Value;
			OutMaxStats.Add(Entries[Index].Stat);
		}

		return Max;
	}

	int32 GetMinStats(TArray<TSoftObjectPtr<UApologueStat>>& OutMinStats) const
	{
		int32 Min = TNumericLimits<int32>::Max();
		OutMinStats.Reset();
		for (int32 Index = 0; Index < Num(); ++Index)
		{
			if (Entries[Index].Value > Min)
			{
				continue;
			}

			if (Entries[Index].Value < Min)
			{
				OutMinStats.Reset();
			}

			Min = Entries[Index].Value;
			OutMinStats.Add(Entries[Index].Stat);
		}

		return Min;
	}

	// Serialization
	friend FArchive& operator<<(FArchive& Ar, FApologueStatTable& StatTable)
	{
		return Ar << StatTable.Entries;
	}

	bool Serialize(FArchive& Ar)
	{
		Ar << *this;
		return true;
	}

	friend void operator<<(const FStructuredArchive::FSlot Slot, FApologueStatTable& StatTable)
	{
		Slot << StatTable.Entries;
	}

	bool Serialize(const FStructuredArchive::FSlot Slot)
	{
		Slot << *this;
		return true;
	}
};

UCLASS()
class UApologueStatTableFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Pokémon Stat Table", meta=(NativeMakeFunc))
	static FApologueStatTable Make(const TArray<FApologueStatTableEntry>& Data)
	{
		// Ensures validation
		return FApologueStatTable(Data);
	}

	UFUNCTION(BlueprintPure, Category="Pokémon Stat Table", meta=(NativeBreakFunc))
	static void Break(const FApologueStatTable& StatTable, UPARAM(DisplayName="Data") TArray<FApologueStatTableEntry>& OutData)
	{
		OutData = StatTable.Entries;
	}

	UFUNCTION(BlueprintPure, Category="Pokémon Stat Table", meta=(AutoCreateRefTerm="StatType"))
	static UPARAM(DisplayName="bSuccess") bool GetStatTableValue(const FApologueStatTable& StatTable, const TSoftObjectPtr<UApologueStat>& StatType, int32& OutValue)
	{
		return StatTable.TryGet(StatType, OutValue);
	}

	UFUNCTION(BlueprintCallable, Category="Pokémon Stat Table", meta=(AutoCreateRefTerm="StatType"))
	static UPARAM(DisplayName="bSuccess") bool SetStatTableValue(UPARAM(ref) FApologueStatTable& StatTable, const TSoftObjectPtr<UApologueStat>& StatType, const int32 NewValue)
	{
		return StatTable.TrySet(StatType, NewValue);
	}

	UFUNCTION(BlueprintPure, Category="Pokémon Stat Table", meta=(CompactNodeTitle="Total"))
	static UPARAM(DisplayName="Total") int32 GetTotal(const FApologueStatTable& StatTable)
	{
		return StatTable.GetTotal();
	}

	UFUNCTION(BlueprintPure, Category="Pokémon Stat Table", meta=(CompactNodeTitle="Count"))
	static UPARAM(DisplayName="Count") int32 GetCount(const FApologueStatTable& StatTable)
	{
		return StatTable.Num();
	}

	UFUNCTION(BlueprintPure, Category="Pokémon Stat Table", meta=(CompactNodeTitle="Count"))
	static UPARAM(DisplayName="Average") float GetAverage(const FApologueStatTable& StatTable)
	{
		return StatTable.Num();
	}

	UFUNCTION(BlueprintPure, Category="Pokémon Stat Table")
	static UPARAM(DisplayName="Max Value") int32 GetMaxStats(const FApologueStatTable& StatTable,
	                                                         UPARAM(DisplayName="Stat Types") TArray<TSoftObjectPtr<UApologueStat>>& OutStatTypes)
	{
		return StatTable.GetMaxStats(OutStatTypes);
	}

	UFUNCTION(BlueprintPure, Category="Pokémon Stat Table")
	static UPARAM(DisplayName="Min Value") int32 GetMinStats(const FApologueStatTable& StatTable,
	                                                         UPARAM(DisplayName="Stat Types") TArray<TSoftObjectPtr<UApologueStat>>& OutStatTypes)
	{
		return StatTable.GetMinStats(OutStatTypes);
	}
};
