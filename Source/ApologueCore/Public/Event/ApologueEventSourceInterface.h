// Copyright (c) 2024 David Jacquish

#pragma once

#include "CoreMinimal.h"
#include "Flag/ApologueFlagHolderInterface.h"
#include "UObject/Interface.h"
#include "ApologueEventSourceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UApologueEventSourceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class APOLOGUECORE_API IApologueEventSourceInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TScriptInterface<IApologueFlagHolderInterface> EventSource_GetFlagHolder();
};
