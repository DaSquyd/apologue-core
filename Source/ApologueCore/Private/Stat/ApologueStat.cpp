// Copyright (c) 2024 David Jacquish


#include "Stat/ApologueStat.h"

#include "Stat/ApologueStatFunction.h"

FApologueStatValue UApologueStat::GetValue() const
{
	return Function->GetValue();
}
