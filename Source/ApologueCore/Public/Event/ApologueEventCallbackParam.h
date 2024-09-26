// Copyright (c) 2024 David Jacquish

#pragma once

#include "ApologueEvent.h"

#include "ApologueEventCallbackParam.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FApologueCallback, const UApologueEventContext*, Context);

USTRUCT(BlueprintType)
struct APOLOGUECORE_API FApologueEventCallbackParam
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UApologueEvent> Event;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FApologueCallback Callback;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SubPriority = 0;

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
	return A.Priority == B.Priority && A.SubPriority == B.SubPriority;
}

inline bool operator!=(const FApologueEventCallbackParam& A, const FApologueEventCallbackParam& B)
{
	check(A.IsValid());
	check(B.IsValid());
	checkf(A.Event == B.Event, UNLIKE_EVENT_COMPARISON);
	return A.Priority != B.Priority || A.SubPriority != B.SubPriority;
}

inline bool operator<(const FApologueEventCallbackParam& A, const FApologueEventCallbackParam& B)
{
	check(A.IsValid());
	check(B.IsValid());
	checkf(A.Event == B.Event, UNLIKE_EVENT_COMPARISON);	
	return A.Priority == B.Priority ? A.SubPriority < B.SubPriority : A.Priority < B.Priority;
}

inline bool operator>(const FApologueEventCallbackParam& A, const FApologueEventCallbackParam& B)
{
	check(A.IsValid());
	check(B.IsValid());
	checkf(A.Event == B.Event, UNLIKE_EVENT_COMPARISON);
	return A.Priority == B.Priority ? A.SubPriority > B.SubPriority : A.Priority > B.Priority;
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
