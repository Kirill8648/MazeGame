// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/Generation/MGBiomeData.h"

UMGBiomeData::UMGBiomeData()
{
	BiomeName = "";
}

UMGBiomeData* UMGBiomeData::GetBiomeByName(const FName BiomeName, TArray<UMGBiomeData*>& Biomes)
{
	for (UMGBiomeData* Biome : Biomes)
		if (Biome->BiomeName.IsEqual(BiomeName))
			return Biome;

	return nullptr;
}
