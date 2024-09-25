// Copyright (c) 2024 David Jacquish

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ApologueFlagHolderInterface.generated.h"

class UApologueFlag;

// This class does not need to be modified.
UINTERFACE()
class UApologueFlagHolderInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class APOLOGUECORE_API IApologueFlagHolderInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool HasFlag(const TSoftObjectPtr<UApologueFlag>& Flag);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool HasAllFlags(const TArray<TSoftObjectPtr<UApologueFlag>>& Flags);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool HasAnyFlag(const TArray<TSoftObjectPtr<UApologueFlag>>& Flags);
};
