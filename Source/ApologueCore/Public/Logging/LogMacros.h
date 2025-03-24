#pragma once

#if WITH_EDITOR
#define AP_LOG(CategoryName, Verbosity, Format, ...) \
{ \
	UE_LOG(CategoryName, Verbosity, Format, ##__VA_ARGS__); \
	FMessageLog("PIE").Info()->AddToken(FTextToken::Create(FText::FromString(FString::Printf(Format, ##__VA_ARGS__) + TEXT(" ") + FString(__FILE__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")")), false)); \
}
#else
#define AP_LOG(CategoryName, Verbosity, Format, ...) \
{ \
	UE_LOG(CategoryName, Verbosity, Format, ##__VA_ARGS__); \
}
#endif

#define AP_CLOG(Condition, CategoryName, Verbosity, Format, ...) \
	if (Condition) { AP_LOG(CategoryName, Verbosity, Format, ##__VA_ARGS__) }
