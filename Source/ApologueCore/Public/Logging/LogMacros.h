#pragma once

#include "Log.h"

#if WITH_EDITOR
#define AP_LOG_MESSAGE_PRIVATE(Severity, Format, ...) \
	FMessageLog("PIE").SuppressLoggingToOutputLog().AddMessage(FTokenizedMessage::Create(::EMessageSeverity::Severity)->AddToken(FTextToken::Create(FText::FromString(FString::Printf(Format, ##__VA_ARGS__) + TEXT(" ") + FString(__FILE__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")")), false)));
#define AP_LOG_FATAL(CategoryName, Format, ...) \
{ \
	AP_LOG_MESSAGE_PRIVATE(Error, Format, ##__VA_ARGS__); \
	UE_LOG(CategoryName, Fatal, Format, ##__VA_ARGS__); \
}
#define AP_LOG_ERROR(CategoryName, Format, ...) \
{ \
	AP_LOG_MESSAGE_PRIVATE(Error, Format, ##__VA_ARGS__); \
	UE_LOG(CategoryName, Error, Format, ##__VA_ARGS__); \
}
#define AP_LOG_WARNING(CategoryName, Format, ...) \
{ \
	AP_LOG_MESSAGE_PRIVATE(Warning, Format, ##__VA_ARGS__); \
	UE_LOG(CategoryName, Warning, Format, ##__VA_ARGS__); \
}
#define AP_LOG_PERF_WARN(CategoryName, Format, ...) \
{ \
	AP_LOG_MESSAGE_PRIVATE(PerformanceWarning, Format, ##__VA_ARGS__); \
	UE_LOG(CategoryName, Warning, Format, ##__VA_ARGS__); \
}
#define AP_LOG(CategoryName, Format, ...) \
{ \
	AP_LOG_MESSAGE_PRIVATE(Info, Format, ##__VA_ARGS__); \
	UE_LOG(CategoryName, Display, Format, ##__VA_ARGS__); \
}
#define AP_LOG_VERBOSE(CategoryName, Format, ...) \
{ \
	UE_LOG(CategoryName, Verbose, Format, ##__VA_ARGS__); \
}
#define AP_LOG_VERY_VERBOSE(CategoryName, Format, ...) \
{ \
	UE_LOG(CategoryName, VeryVerbose, Format, ##__VA_ARGS__); \
}
#else
#define AP_LOG_FATAL(CategoryName, Format, ...) \
{ \
	UE_LOG(CategoryName, Fatal, Format, ##__VA_ARGS__); \
}
#define AP_LOG_ERROR(CategoryName, Format, ...) \
{ \
	UE_LOG(CategoryName, Error, Format, ##__VA_ARGS__); \
}
#define AP_LOG_WARNING(CategoryName, Format, ...) \
{ \
	UE_LOG(CategoryName, Warning, Format, ##__VA_ARGS__); \
}
#define AP_LOG_PERF_WARN(CategoryName, Format, ...) \
{ \
	UE_LOG(CategoryName, Warning, Format, ##__VA_ARGS__); \
}
#define AP_LOG(CategoryName, Format, ...) \
{ \
	UE_LOG(CategoryName, Display, Format, ##__VA_ARGS__); \
}
#define AP_LOG_VERBOSE(CategoryName, Format, ...) \
{ \
	UE_LOG(CategoryName, Verbose, Format, ##__VA_ARGS__); \
}
#define AP_LOG_VERY_VERBOSE(CategoryName, Format, ...) \
{ \
	UE_LOG(CategoryName, VeryVerbose, Format, ##__VA_ARGS__); \
}
#endif

#define AP_CLOG_FATAL(Condition, CategoryName, Format, ...) \
	{ if (Condition) { AP_LOG_FATAL(CategoryName, Format, ##__VA_ARGS__) } }
#define AP_CLOG_ERROR(Condition, CategoryName, Format, ...) \
	{ if (Condition) { AP_LOG_ERROR(CategoryName, Format, ##__VA_ARGS__) } }
#define AP_CLOG_WARNING(Condition, CategoryName, Format, ...) \
	{ if (Condition) { AP_LOG_WARNING(CategoryName, Format, ##__VA_ARGS__) } }
#define AP_CLOG_PERF_WARNING(Condition, CategoryName, Format, ...) \
	{ if (Condition) { AP_LOG_PERF_WARNING(CategoryName, Format, ##__VA_ARGS__) } }
#define AP_CLOG(Condition, CategoryName, Format, ...) \
	{ if (Condition) { AP_LOG(CategoryName, Format, ##__VA_ARGS__) } }
#define AP_CLOG_VERBOSE(Condition, CategoryName, Format, ...) \
	{ if (Condition) { AP_LOG_VERBOSE(CategoryName, Format, ##__VA_ARGS__) } }
#define AP_CLOG_VERY_VERBOSE(Condition, CategoryName, Format, ...) \
	{ if (Condition) { AP_LOG_VERY_VERBOSE(CategoryName, Format, ##__VA_ARGS__) } }
