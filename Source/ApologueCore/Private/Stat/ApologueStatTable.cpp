#include "Stat/ApologueStatTable.h"

#include "Log.h"
#include "ApologueCore/Public/Logging/LogMacros.h"

FApologueStatTable::FApologueStatTable(const TArray<TSoftObjectPtr<UApologueStat>>& Stats, const int32 DefaultValue, const bool bKeepExistingValues)
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

FApologueStatTable::FApologueStatTable(const TArray<FApologueStatTableEntry>& InEntries)
{
	Entries.Reserve(InEntries.Num());
	for (const FApologueStatTableEntry& Entry : InEntries)
	{
		if (ensure(!Has(Entry.Stat)))
		{
			Entries.Add(Entry);
		}
	}
}

bool FApologueStatTable::Has(const TSoftObjectPtr<UApologueStat>& Stat) const
{
	auto Predicate = [&Stat](const FApologueStatTableEntry& Entry)-> bool
	{
		return Entry.Stat == Stat;
	};

	return !!Entries.FindByPredicate(Predicate);
}

int32& FApologueStatTable::Get(const TSoftObjectPtr<UApologueStat>& StatType)
{
	auto Predicate = [&StatType](const FApologueStatTableEntry& Entry)-> bool
	{
		return Entry.Stat == StatType;
	};

	FApologueStatTableEntry* EntryPtr = Entries.FindByPredicate(Predicate);

	check(EntryPtr);
	return EntryPtr->Value;
}

bool FApologueStatTable::TryGet(const TSoftObjectPtr<UApologueStat>& Stat, int32& OutValue) const
{
	auto Predicate = [&Stat](const FApologueStatTableEntry& Entry)-> bool
	{
		return Entry.Stat == Stat;
	};

	if (const FApologueStatTableEntry* EntryPtr = Entries.FindByPredicate(Predicate))
	{
		OutValue = EntryPtr->Value;
		return true;
	}

	OutValue = int32();
	return false;
}

bool FApologueStatTable::TrySet(const TSoftObjectPtr<UApologueStat>& Stat, const int32& NewValue)
{
	auto Predicate = [&Stat](const FApologueStatTableEntry& Entry)-> bool
	{
		return Entry.Stat == Stat;
	};

	if (FApologueStatTableEntry* EntryPtr = Entries.FindByPredicate(Predicate))
	{
		EntryPtr->Value = NewValue;
		return true;
	}

	return false;
}

void FApologueStatTable::ForEach(const TFunctionRef<void(const TSoftObjectPtr<UApologueStat>& Stat, const int32& Value, bool& bBreak)>& Function) const
{
	for (const FApologueStatTableEntry& Entry : Entries)
	{
		bool bBreak = false;
		Function(Entry.Stat, Entry.Value, bBreak);
		if (bBreak)
		{
			return;
		}
	}
}

void FApologueStatTable::ForEach(const TFunctionRef<void(const TSoftObjectPtr<UApologueStat>& Stat, const int32& Value)>& Function, const bool* BreakPtr) const
{
	for (const FApologueStatTableEntry& Entry : Entries)
	{
		if (BreakPtr && *BreakPtr)
		{
			return;
		}

		Function(Entry.Stat, Entry.Value);
	}
}

void FApologueStatTable::ForEach(const TFunctionRef<void(const TSoftObjectPtr<UApologueStat>& StatType, int32& Value, bool& bBreak)>& Function)
{
	for (FApologueStatTableEntry& Entry : Entries)
	{
		bool bBreak = false;
		Function(Entry.Stat, Entry.Value, bBreak);
		if (bBreak)
		{
			return;
		}
	}
}

void FApologueStatTable::ForEach(const TFunctionRef<void(const TSoftObjectPtr<UApologueStat>& StatType, int32& Value)>& Function, const bool* BreakPtr)
{
	for (FApologueStatTableEntry& Entry : Entries)
	{
		if (BreakPtr && *BreakPtr)
		{
			return;
		}

		Function(Entry.Stat, Entry.Value);
	}
}

bool FApologueStatTable::Validate() const
{
	for (int32 I = 0; I < Num(); I++)
	{
		for (int32 J = I + 1; J < Num(); J++)
		{
			if (Entries[I].Stat == Entries[J].Stat)
			{
				return false;
			}
		}
	}
	return true;
}

void FApologueStatTable::GetValues(TArray<int32>& OutValues)
{
	OutValues.Reset(Num());
	for (FApologueStatTableEntry& Entry : Entries)
	{
		OutValues.Add(Entry.Value);
	}
}

int32 FApologueStatTable::GetTotal() const
{
	int32 Total = 0;
	for (const FApologueStatTableEntry& Entry : Entries)
	{
		Total += Entry.Value;
	}

	return Total;
}

double FApologueStatTable::GetAverage() const
{
	double Total = 0;
	for (const FApologueStatTableEntry& Entry : Entries)
	{
		Total += Entry.Value;
	}

	return Total / Num();
}

int32 FApologueStatTable::GetMaxValue() const
{
	if (Num() == 0)
	{
		AP_LOG(LogApologueCore, Error, TEXT("Cannot get max value from an empty Stat Table"))
		return 0;
	}

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

int32 FApologueStatTable::GetMinValue() const
{
	if (Num() == 0)
	{
		AP_LOG(LogApologueCore, Error, TEXT("Cannot get min value from an empty Stat Table"))
		return 0;
	}

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

int32 FApologueStatTable::GetMaxStats(TArray<TSoftObjectPtr<UApologueStat>>& OutMaxStats) const
{
	OutMaxStats.Reset();

	if (Num() == 0)
	{
		AP_LOG(LogApologueCore, Error, TEXT("Cannot get max stats from an empty Stat Table"))
		return 0;
	}

	int32 Max = TNumericLimits<int32>::Min();
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

int32 FApologueStatTable::GetMinStats(TArray<TSoftObjectPtr<UApologueStat>>& OutMinStats) const
{
	OutMinStats.Reset();

	if (Num() == 0)
	{
		AP_LOG(LogApologueCore, Error, TEXT("Cannot get min stats from an empty Stat Table"))
		return 0;
	}

	int32 Min = TNumericLimits<int32>::Max();
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
