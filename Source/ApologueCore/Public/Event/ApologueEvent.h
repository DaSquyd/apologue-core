// Copyright (c) 2024 David Jacquish

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ApologueEvent.generated.h"

class UApologueEventContext;
/**
 * 
 */
UCLASS(BlueprintType)
class APOLOGUECORE_API UApologueEvent : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TSoftClassPtr<UApologueEventContext> ContextClass;
};
