// Copyright (c) 2024 David Jacquish

#pragma once

#include "ApologueEvent.h"
#include "ApologueEventBroadcasterInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ApologueEventParam.generated.h"

DECLARE_DYNAMIC_DELEGATE(FApologueEventCompleteCallback);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FApologueEventCallback, TScriptInterface<IApologueEventBroadcasterInterface>, EventBroadcaster, const UApologueEventContext*, Context, bool&, bIsDeferred);

USTRUCT(BlueprintType)
struct APOLOGUECORE_API FApologueEventParam
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Apologue Event Param")
	TSoftObjectPtr<UApologueEvent> Event;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Apologue Event Param")
	FApologueEventCallback Callback;

	bool IsValid() const
	{
		return !Event.IsNull() && Callback.IsBound();
	}
};

UCLASS()
class APOLOGUECORE_API UApologueEventParamLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Apologue|Event|Event Param")
	static bool IsValid(const FApologueEventParam& EventParam)
	{
		return EventParam.IsValid();
	}
};
