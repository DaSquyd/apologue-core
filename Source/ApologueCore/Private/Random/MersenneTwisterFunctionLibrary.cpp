// Fill out your copyright notice in the Description page of Project Settings.


#include "Random/MersenneTwisterFunctionLibrary.h"

#include "Random/MersenneTwister.h"

bool UMersenneTwisterFunctionLibrary::GetState(const FMersenneTwister& Engine, TArray<int64>& State, int32& Index)
{
	if (!Engine.IsInitialized())
		return false;

	Engine.GetState(reinterpret_cast<TArray<FMersenneTwister::FEngineType::result_type>&>(State));
	Index = Engine.GetStateIndex();
	return true;
}
