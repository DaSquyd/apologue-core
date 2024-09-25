// Copyright (c) 2024 David Jacquish

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ApologueEventBroadcaster.generated.h"

class UApologueEvent;
class UApologueEventContext;

// This class does not need to be modified.
UINTERFACE()
class UApologueEventBroadcaster : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class APOLOGUECORE_API IApologueEventBroadcaster
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BroadcastEvent(const TSoftObjectPtr<UApologueEvent>& Event, const UApologueEventContext* Context);
};
