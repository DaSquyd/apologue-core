// Copyright (c) 2024 David Jacquish

#pragma once

#include "ApologueEvent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ApologueEventCallbackParam.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FApologueCallback, const UApologueEventContext*, Context);

USTRUCT(BlueprintType)
struct APOLOGUECORE_API FApologueEventCallbackParam
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Apologue Event Callback Param")
	TSoftObjectPtr<UApologueEvent> Event;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Apologue Event Callback Param")
	FApologueCallback Callback;

	// The following Priority, SubPriority, and Order are used for sorting callbacks. 

	// Highest level prioritization; best used for major turn order overrides (e.g., battle phases)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Apologue Event Callback Param")
	int32 Priority = 0;

	// Highest level prioritization; best used for minor turn order overrides (e.g., attacks that always go first)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Apologue Event Callback Param")
	int32 SubPriority = 0;

	// Lowest level prioritization; best used for standard turn ordering (e.g., a Fighter's speed stat)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Apologue Event Callback Param")
	int32 Order = 0;

	bool IsValid() const
	{
		return !Event.IsNull() && Callback.IsBound();
	}
};

#define UNLIKE_EVENT_COMPARISON TEXT("Unable to compare unlike events")

inline bool operator==(const FApologueEventCallbackParam& A, const FApologueEventCallbackParam& B)
{
	check(A.IsValid());
	check(B.IsValid());
	checkf(A.Event == B.Event, UNLIKE_EVENT_COMPARISON);
	return A.Priority == B.Priority && A.SubPriority == B.SubPriority && A.Order == B.Order;
}

inline bool operator!=(const FApologueEventCallbackParam& A, const FApologueEventCallbackParam& B)
{
	check(A.IsValid());
	check(B.IsValid());
	checkf(A.Event == B.Event, UNLIKE_EVENT_COMPARISON);
	return A.Priority != B.Priority || A.SubPriority != B.SubPriority || A.Order != B.Order;
}

inline bool operator<(const FApologueEventCallbackParam& A, const FApologueEventCallbackParam& B)
{
	check(A.IsValid());
	check(B.IsValid());
	checkf(A.Event == B.Event, UNLIKE_EVENT_COMPARISON);
	if (A.Priority == B.Priority)
	{
		if (A.SubPriority == B.SubPriority)
		{
			return A.Order < B.Order;
		}

		return A.SubPriority < B.SubPriority;
	}

	return A.Priority < B.Priority;
}

inline bool operator>(const FApologueEventCallbackParam& A, const FApologueEventCallbackParam& B)
{
	check(A.IsValid());
	check(B.IsValid());
	checkf(A.Event == B.Event, UNLIKE_EVENT_COMPARISON);
	if (A.Priority == B.Priority)
	{
		if (A.SubPriority == B.SubPriority)
		{
			return A.Order > B.Order;
		}

		return A.SubPriority > B.SubPriority;
	}

	return A.Priority > B.Priority;
}

inline bool operator<=(const FApologueEventCallbackParam& A, const FApologueEventCallbackParam& B)
{
	check(A.IsValid());
	check(B.IsValid());
	checkf(A.Event == B.Event, UNLIKE_EVENT_COMPARISON);
	if (A.Priority == B.Priority)
	{
		if (A.SubPriority == B.SubPriority)
		{
			return A.Order <= B.Order;
		}

		return A.SubPriority < B.SubPriority;
	}

	return A.Priority < B.Priority;
}

inline bool operator>=(const FApologueEventCallbackParam& A, const FApologueEventCallbackParam& B)
{
	check(A.IsValid());
	check(B.IsValid());
	checkf(A.Event == B.Event, UNLIKE_EVENT_COMPARISON);
	if (A.Priority == B.Priority)
    {
    	if (A.SubPriority == B.SubPriority)
    	{
    		return A.Order >= B.Order;
    	}

		return A.SubPriority > B.SubPriority;
    }

	return A.Priority > B.Priority;
}

#undef UNLIKE_EVENT_COMPARISON

UCLASS()
class APOLOGUECORE_API UApologueEventCallbackParamLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Apologue|Event|Event Callback Param")
	static bool IsValid(const FApologueEventCallbackParam& CallbackParam)
	{
		return CallbackParam.IsValid();
	}
};
