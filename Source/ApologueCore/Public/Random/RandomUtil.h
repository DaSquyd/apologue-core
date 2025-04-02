// Copyright (c) 2025 David Jacquish

#pragma once

#include <random>

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

		// Explicitly check for 0 because this is a potential fail value for FCString::Strtoi64() and we don't want any false negatives.
		const FRegexPattern ZeroPattern(TEXT(R"(^\s*(\+|-)?(0(x|X))?0+$)"));
		FRegexMatcher ZeroMatcher(ZeroPattern, StringSeed);
		if (ZeroMatcher.FindNext())
		{
			return 0;
		}

		const FRegexPattern HexadecimalPattern(TEXT(R"(^\s*(\+|-)?0(x|X)[0-9a-fA-F]+$)"));
		FRegexMatcher HexadecimalMatcher(HexadecimalPattern, StringSeed);
		if (HexadecimalMatcher.FindNext())
		{
			const int64 HexadecimalSeed = FCString::Strtoi64(*StringSeed, nullptr, 16);
			if (HexadecimalSeed != 0 && HexadecimalSeed != -1)
			{
				return HexadecimalSeed;
			}
		}

		const int64 DecimalSeed = FCString::Strtoi64(*StringSeed, nullptr, 10);
		if (DecimalSeed != 0 && DecimalSeed != -1)
		{
			return DecimalSeed;
		}

		CityHash64(GetData(StringSeed), StringSeed.IsNumeric());
		return GetTypeHash(StringSeed);
	}
};
