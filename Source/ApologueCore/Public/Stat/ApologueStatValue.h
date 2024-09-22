// Copyright (c) 2024 David Jacquish

#pragma once
#include "Containers/Union.h"

#include "ApologueStatValue.generated.h"

USTRUCT(BlueprintType, DisplayName="Stat Value")
struct APOLOGUECORE_API FApologueStatValue
{
	GENERATED_BODY()

private:
	TUnion<int32, float> Value;

public:
	bool IsValid() const
	{
		return Value.GetCurrentSubtypeIndex() != static_cast<uint8>(INDEX_NONE);
	}

	template <typename T>
	bool IsType() const
	{
		return Value.HasSubtype<T>();
	}

	template <typename T>
	bool TryGetValue(T& OutValue) const
	{
		if (IsType<T>())
		{
			OutValue = Value.GetSubtype<T>();
			return true;
		}

		return false;
	}

	template <typename T>
	T GetValue() const
	{
		return Value.GetSubtype<T>();
	}

	void Serialize(FArchive& Ar)
	{
		Ar << Value;
	}
};

inline FArchive& operator<<(FArchive& Ar, FApologueStatValue& StatValue) {
	StatValue.Serialize(Ar);
	return Ar;
}

UCLASS()
class APOLOGUECORE_API UApologueStatValueFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Stat Value")
	static bool IsValid(const FApologueStatValue& StatValue)
	{
		return StatValue.IsValid();
	}

	UFUNCTION(BlueprintPure, Category="Stat Value", DisplayName="Get Value (Integer)")
	UPARAM(DisplayName="bSuccess") static bool GetValue_INT32(const FApologueStatValue& StatValue, UPARAM(DisplayName="Value") int32& OutValue)
	{
		return StatValue.TryGetValue(OutValue);
	}
	
	UFUNCTION(BlueprintPure, Category="Stat Value", DisplayName="Get Value (Float)")
	UPARAM(DisplayName="bSuccess") static bool GetValue_Float(const FApologueStatValue& StatValue, UPARAM(DisplayName="Value") float& OutValue)
	{
		return StatValue.TryGetValue(OutValue);
	}
};
