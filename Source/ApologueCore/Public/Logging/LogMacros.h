#pragma once

#define APOLOGUE_RETURN_WARNING_V(Condition, FailReturnValue, CategoryName, Format, ...) \
{ \
	if (!(Condition)) \
	{ \
		UE_LOG(CategoryName, Warning, Format, ##__VA_ARGS__) \
		return FailReturnValue; \
	} \
}

#define APOLOGUE_RETURN_ERROR(Condition, FailReturnValue, CategoryName, Format, ...) \
{ \
	if (!(Condition)) \
	{ \
		UE_LOG(CategoryName, Error, Format, ##__VA_ARGS__) \
		return FailReturnValue; \
	} \
}

#define APOLOGUE_RETURN_FATAL(Condition, FailReturnValue, CategoryName, Format, ...) \
{ \
	if (!(Condition)) \
	{ \
		UE_LOG(CategoryName, Fatal, Format, ##__VA_ARGS__) \
		return FailReturnValue; \
	} \
}

#define APOLOGUE_RETURN_ENSURE(Condition, FailReturnValue, CategoryName, Format, ...) \
{ \
	if (!ensureMsgf(Condition, Format, ##__VA_ARGS__)) \
	{ \
		UE_LOG(CategoryName, Error, Format, ##__VA_ARGS__) \
		return FailReturnValue; \
	} \
}