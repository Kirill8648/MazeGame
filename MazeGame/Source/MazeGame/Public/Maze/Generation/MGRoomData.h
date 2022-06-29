// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MGRoomData.generated.h"

/**
 * 
 */
UCLASS()
class MAZEGAME_API UMGRoomData : public UDataAsset
{
	GENERATED_BODY()

public:
	UMGRoomData();
	
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> Level;
	UPROPERTY(EditAnywhere)
	UStaticMesh* DistanceMesh;
	/*
	 * Room Chunks coordinates:
	 *			+Y
	 * -X					+X
	 *			-Y
	 * The first element is the root chunk and should be equal to 0.0f, 0.0f
	 * In a common room there will be only one chunk with coordinates 0.0f, 0.0f
	 */
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0, ClampMax = 10, Multiple = 1, Delta = 1))
	TSet<FVector2D> RoomChunks = {FVector2D(0.0f, 0.0f)};
};
