// Copyright (c) 2024 David Jacquish

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ApologueEvent.generated.h"

class UApologueEventContext;
class UApologueEventSortHandler;

/**
 * 
 */
UCLASS(BlueprintType)
class APOLOGUECORE_API UApologueEvent : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess, MustImplement="/Script/ApologueCore.ApologueEventListenerInterface"))
	TSoftClassPtr<UObject> ListenerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	TSoftClassPtr<UApologueEventContext> ContextClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, meta=(AllowPrivateAccess, ShowOnlyInnerProperties))
	TObjectPtr<UApologueEventSortHandler> SortHandler;

public:
	TSoftClassPtr<UObject> GetListenerClass() const { return ListenerClass; }
	TSoftClassPtr<UApologueEventContext> GetContextClass() const { return ContextClass; }
};
