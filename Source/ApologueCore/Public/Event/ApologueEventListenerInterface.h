// Copyright (c) 2024 David Jacquish

#pragma once

#include "CoreMinimal.h"
#include "ApologueCallbackParam.h"
#include "UObject/Interface.h"
#include "ApologueEventListenerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UApologueEventListenerInterface : public UInterface
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
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void GetCallbacks(TArray<FApologueCallbackParam>& OutCallbackParams);
};
