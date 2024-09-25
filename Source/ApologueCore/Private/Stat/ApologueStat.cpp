// Copyright (c) 2024 David Jacquish


#include "Stat/ApologueStat.h"

#include "Stat/ApologueStatFunction.h"

int32 UApologueStat::GetValue(const int32 BaseValue, const UStatFunctionContext* Context) const
{
	return Function->GetValue(this, BaseValue, Context);
}
