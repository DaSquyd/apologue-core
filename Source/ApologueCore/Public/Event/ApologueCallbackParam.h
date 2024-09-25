// Copyright (c) 2024 David Jacquish

#pragma once

#include "ApologueEvent.h"
#include "ApologueEventContext.h"

#include "ApologueCallbackParam.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FApologueCallback, const UApologueEventContext*, Context);

USTRUCT(BlueprintType)
struct APOLOGUECORE_API FApologueCallbackParam
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TSoftObjectPtr<UApologueEvent> Event;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	FApologueCallback Callback;

public:
	FApologueCallbackParam()
	{
	}

	FApologueCallbackParam(const TSoftObjectPtr<UApologueEvent>& Event, const FApologueCallback& Callback)
		: Event(Event), Callback(Callback)
	{
	}

	bool IsValid() const
	{
		return !Event.IsNull() && Callback.IsBound();
	}

	const TSoftObjectPtr<UApologueEvent>& GetEvent()
	{
		return Event;
	}

	const FApologueCallback& GetCallback()
	{
		return Callback;
	}
};
