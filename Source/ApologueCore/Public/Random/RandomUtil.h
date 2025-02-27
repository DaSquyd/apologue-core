// Copyright (c) 2025 David Jacquish

#pragma once

struct FRandomUtil
{
	static int64 GetSeedFromHardware()
	{
		std::random_device RandomDevice;
		return RandomDevice();
	}
	
	static int64 GetSeedFromString(const FString& StringSeed)
	{
		if (StringSeed.IsEmpty())
		{
			return GetSeedFromHardware();
		}

		// Explicitly check for 0 or -1 because these are potential fail values for FCString::Strtoi64() and we don't want any false negatives.
		const FRegexPattern ZeroPattern(TEXT(R"(^(\+|-)?0+$)"));
		FRegexMatcher ZeroMatcher(ZeroPattern, StringSeed);
		if (ZeroMatcher.FindNext())
		{
			return 0;
		}

		const FRegexPattern NegativeOnePattern(TEXT(R"(^-[0]*1$)"));
		FRegexMatcher NegativeOneMatcher(ZeroPattern, StringSeed);
		if (NegativeOneMatcher.FindNext())
		{
			return -1;
		}

		const int64 IntSeed = FCString::Strtoi64(*StringSeed, nullptr, 10);
		if (IntSeed != 0 && IntSeed != -1)
		{
			return IntSeed;
		}
		
		return GetTypeHash(StringSeed);
	}
};