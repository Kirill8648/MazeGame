// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MGRoomData.h"
#include "Engine/DataAsset.h"
#include "Engine/LevelStreamingDynamic.h"
#include "MGBiomeData.generated.h"


UENUM(BlueprintType)
enum ENoiseMapTypes
{
	Height UMETA(DisplayName = "Height"),
	Heat UMETA(DisplayName = "Heat"),
	Moisture UMETA(DisplayName = "Moisture"),
};

UENUM(BlueprintType)
enum ERoomSpawnType
{
	/*
	 * Spawn rate as a percentage of the biome grid from 0 to 100, which will be filled with this room + considering weights.
	 * It is considered in the second place.
	 */
	SpawnRate UMETA(DisplayName = "SpawnRate"),
	/*
	 * The NumberOfRooms is the exact amount of spawned rooms + considering weights.
	 * It is considered first of all.
	 * Guarantees the spawn of rooms, if at least one room has not been spawned for some reason, the algorithm will display a warning on the screen.
	 */
	NumberOfRooms UMETA(DisplayName = "NumberOfRooms"),
	/*
	 * Spawn weight of random spawn in empty remaining cells + considering weights.
	 * It is considered in the third turn.
	 * DOES NOT GUARANTEE THE SPAWN OF ROOMS.
	 */
	SpawnWeight UMETA(DisplayName = "SpawnWeight"),
};

USTRUCT()
struct FRoomInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	UMGRoomData* Room;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 100, EditCondition = "SpawnType==ERoomSpawnType::SpawnRate", EditConditionHides, Units="Percent"))
	float SpawnRate = 0.0f;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "SpawnType==ERoomSpawnType::NumberOfRooms", EditConditionHides, Units="times"))
	int32 NumberOfRooms = 0;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "SpawnType==ERoomSpawnType::SpawnWeight", EditConditionHides))
	int32 SpawnWeight = 0;
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ERoomSpawnType> SpawnType = ERoomSpawnType::SpawnRate;
	
	UPROPERTY(EditAnywhere)
	bool bShowOptionalProperties = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = bShowOptionalProperties, EditConditionHides))
	/*Weights are optional and will be used only if the TMap is not empty*/
	TMap<TEnumAsByte<ENoiseMapTypes>, float> OptionalWeights;
	UPROPERTY(EditAnywhere, meta = (EditCondition = bShowOptionalProperties, EditConditionHides))
	FVector OptionalOffset;
	/*WARNING! Try to make as few such rooms as possible*/
	UPROPERTY(EditAnywhere, meta = (EditCondition = bShowOptionalProperties, EditConditionHides))
	bool bIsFlat = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = bShowOptionalProperties, EditConditionHides))
	bool bRandomizeRotation = false;
};


UCLASS()
class MAZEGAME_API UMGBiomeData : public UDataAsset
{
	GENERATED_BODY()

public:
	UMGBiomeData();

	UPROPERTY(EditAnywhere, Category = "BiomeInfo")
	FName BiomeName;
	UPROPERTY(EditAnywhere, Category = "BiomeInfo")
	TMap<TEnumAsByte<ENoiseMapTypes>, float> Weights;
	UPROPERTY(EditAnywhere, Category = "BiomeInfo", meta = (HideAlphaChannel))
	FColor DebugColor = FColor(0, 0, 0, 255);
	UPROPERTY(EditAnywhere, Category = "Rooms",
		meta = (TitleProperty = "{SpawnType}; SpawnRate: {SpawnRate} %, NumberOfRooms: {NumberOfRooms} x, SpawnWeight: {SpawnWeight}"))
	TArray<FRoomInfo> Rooms;

	static UMGBiomeData* GetBiomeByName(const FName BiomeName, TArray<UMGBiomeData*>& Biomes);
};
