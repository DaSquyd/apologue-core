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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Apologue Stat Table Entry")
	TSoftObjectPtr<UApologueStat> Stat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Apologue Stat Table Entry")
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


USTRUCT(BlueprintType, meta=(HasNativeMake="/Script/ApologueCore.ApologueStatTableFunctionLibrary:Make", HasNativeBreak="/Script/ApologueCore.ApologueStatTableFunctionLibrary:Break"))
struct FApologueStatTable
{
	GENERATED_BODY()

	friend class UApologueStatTableFunctionLibrary;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category="Apologue Stat Table")
	TArray<FApologueStatTableEntry> Entries;

public:
	FApologueStatTable()
	{
	}

	explicit FApologueStatTable(const TArray<TSoftObjectPtr<UApologueStat>>& Stats, const int32 DefaultValue, const bool bKeepExistingValues = false);

	explicit FApologueStatTable(const TArray<FApologueStatTableEntry>& InEntries);

	FORCEINLINE bool IsValid() const
	{
		return !Entries.IsEmpty();
	}

	FORCEINLINE void Reset()
	{
		Entries.Reset();
	}

	bool Has(const TSoftObjectPtr<UApologueStat>& Stat) const;
	int32& Get(const TSoftObjectPtr<UApologueStat>& StatType);

	bool TryGet(const TSoftObjectPtr<UApologueStat>& Stat, int32& OutValue) const;
	bool TrySet(const TSoftObjectPtr<UApologueStat>& Stat, const int32& NewValue);

	void ForEach(const TFunctionRef<void (const TSoftObjectPtr<UApologueStat>& Stat, const int32& Value, bool& bBreak)>& Function) const;
	void ForEach(const TFunctionRef<void (const TSoftObjectPtr<UApologueStat>& Stat, const int32& Value)>& Function, const bool* BreakPtr = nullptr) const;
	void ForEach(const TFunctionRef<void (const TSoftObjectPtr<UApologueStat>& StatType, int32& Value, bool& bBreak)>& Function);
	void ForEach(const TFunctionRef<void (const TSoftObjectPtr<UApologueStat>& StatType, int32& Value)>& Function, const bool* BreakPtr = nullptr);

	bool Validate() const;

	void GetValues(TArray<int32>& OutValues);

	int32 GetTotal() const;

	FORCEINLINE int32 Num() const
	{
		return Entries.Num();
	}

	double GetAverage() const;
	int32 GetMaxValue() const;
	int32 GetMinValue() const;
	int32 GetMaxStats(TArray<TSoftObjectPtr<UApologueStat>>& OutMaxStats) const;
	int32 GetMinStats(TArray<TSoftObjectPtr<UApologueStat>>& OutMinStats) const;

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
	UFUNCTION(BlueprintPure, Category="Apologue Stat Table", meta=(NativeMakeFunc))
	static FApologueStatTable Make(const TArray<FApologueStatTableEntry>& Data)
	{
		// Ensures validation
		return FApologueStatTable(Data);
	}

	UFUNCTION(BlueprintPure, Category="Apologue Stat Table", meta=(NativeBreakFunc))
	static void Break(const FApologueStatTable& StatTable, UPARAM(DisplayName="Data") TArray<FApologueStatTableEntry>& OutData)
	{
		OutData = StatTable.Entries;
	}

	UFUNCTION(BlueprintPure, Category="Apologue Stat Table", meta=(AutoCreateRefTerm="StatType"))
	static UPARAM(DisplayName="Success") bool GetStatTableValue(const FApologueStatTable& StatTable, const TSoftObjectPtr<UApologueStat>& StatType, int32& OutValue)
	{
		return StatTable.TryGet(StatType, OutValue);
	}

	UFUNCTION(BlueprintCallable, Category="Apologue Stat Table", meta=(AutoCreateRefTerm="StatType"))
	static UPARAM(DisplayName="Success") bool SetStatTableValue(UPARAM(ref) FApologueStatTable& StatTable, const TSoftObjectPtr<UApologueStat>& StatType, const int32 NewValue)
	{
		return StatTable.TrySet(StatType, NewValue);
	}

	UFUNCTION(BlueprintPure, Category="Apologue Stat Table", meta=(CompactNodeTitle="Total"))
	static UPARAM(DisplayName="Total") int32 GetTotal(const FApologueStatTable& StatTable)
	{
		return StatTable.GetTotal();
	}

	UFUNCTION(BlueprintPure, Category="Apologue Stat Table", meta=(CompactNodeTitle="Count"))
	static UPARAM(DisplayName="Count") int32 GetCount(const FApologueStatTable& StatTable)
	{
		return StatTable.Num();
	}

	UFUNCTION(BlueprintPure, Category="Apologue Stat Table", meta=(CompactNodeTitle="Count"))
	static UPARAM(DisplayName="Average") float GetAverage(const FApologueStatTable& StatTable)
	{
		return StatTable.Num();
	}

	UFUNCTION(BlueprintPure, Category="Apologue Stat Table")
	static UPARAM(DisplayName="Max Value") int32 GetMaxStats(const FApologueStatTable& StatTable, UPARAM(DisplayName="Stat Types") TArray<TSoftObjectPtr<UApologueStat>>& OutStatTypes)
	{
		return StatTable.GetMaxStats(OutStatTypes);
	}

	UFUNCTION(BlueprintPure, Category="Apologue Stat Table")
	static UPARAM(DisplayName="Min Value") int32 GetMinStats(const FApologueStatTable& StatTable, UPARAM(DisplayName="Stat Types") TArray<TSoftObjectPtr<UApologueStat>>& OutStatTypes)
	{
		return StatTable.GetMinStats(OutStatTypes);
	}
};
