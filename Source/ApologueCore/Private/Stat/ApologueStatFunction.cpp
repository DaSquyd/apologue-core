// Copyright (c) 2024 David Jacquish


#include "Stat/ApologueStatFunction.h"

int32 UApologueStatFunction::GetValue_Implementation(const TSoftObjectPtr<UApologueStat>& Stat, const int32 BaseValue, const UStatFunctionContext* Context)
{
	return BaseValue;
}
