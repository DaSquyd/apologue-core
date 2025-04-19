// Copyright (c) 2025 David Jacquish

#pragma once

#include "CoreMinimal.h"
#include "ApologueEventParam.h"
#include "UObject/Interface.h"
#include "ApologueEventListenerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class APOLOGUECORE_API UApologueEventListenerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class APOLOGUECORE_API IApologueEventListenerInterface
{
	GENERATED_BODY()
	
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Apologue Event Listener Interface")
	void EventListener_GetCallbacks(TArray<FApologueEventParam>& OutCallbackParams);
};
