// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MersenneTwister.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MersenneTwisterFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class APOLOGUECORE_API UMersenneTwisterFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, Category="Apologue|Random|Mersenne Twister")
	static bool GetState(UPARAM(Ref) const FMersenneTwister& Engine, TArray<int64>& State, int32& Index);
};
