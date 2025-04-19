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
UCLASS()
class APOLOGUECORE_API UApologueEvent : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Apologue Event", meta=(AllowPrivateAccess, MustImplement="/Script/ApologueCore.ApologueEventListenerInterface"))
	TSoftClassPtr<UObject> ListenerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Apologue Event", meta=(AllowPrivateAccess))
	TSoftClassPtr<UApologueEventContext> ContextClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category="Apologue Event", meta=(AllowPrivateAccess, ShowOnlyInnerProperties))
	TObjectPtr<UApologueEventSortHandler> SortHandler;

public:
	TSoftClassPtr<UObject> GetListenerClass() const { return ListenerClass; }
	TSoftClassPtr<UApologueEventContext> GetContextClass() const { return ContextClass; }
	UApologueEventSortHandler* GetSortHandler() const { return SortHandler; }
};
