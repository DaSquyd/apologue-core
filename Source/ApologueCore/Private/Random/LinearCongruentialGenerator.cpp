#include "Random/LinearCongruentialGenerator.h"

void FLinearCongruentialGenerator::Discard(const int32 Count)
{
	for (int32 Index = 0; Index < Count; ++Index)
	{
		Mutate();
	}
}
