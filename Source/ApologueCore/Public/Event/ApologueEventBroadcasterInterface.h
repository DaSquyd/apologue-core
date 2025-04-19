// Copyright (c) 2024 David Jacquish

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ApologueEventBroadcasterInterface.generated.h"

class UApologueEvent;
class UApologueEventContext;

DECLARE_DYNAMIC_DELEGATE(FApologueBroadcastEventFinishedCallback);

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UApologueEventBroadcasterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class APOLOGUECORE_API IApologueEventBroadcasterInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Apologue Event Broadcaster Interface")
	void EventBroadcaster_BroadcastEvent(const TSoftObjectPtr<UApologueEvent>& Event, UApologueEventContext* Context, const FApologueBroadcastEventFinishedCallback& Callback);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Apologue Event Broadcaster Interface")
	void EventBroadcaster_CompleteDeferred();
};
