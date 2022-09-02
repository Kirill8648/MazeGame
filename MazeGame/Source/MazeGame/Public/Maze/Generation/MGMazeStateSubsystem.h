// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MGMazeStateSubsystem.generated.h"

struct FRoomInfo;
USTRUCT()
struct FObjectState
{
	GENERATED_BODY()
	int32 MainState;
	TArray<float> AdditionalInfo;
	TArray<FName> Tags;
};

USTRUCT()
struct FDynamicObjectState
{
	GENERATED_BODY()
	int32 MainState;
	TArray<float> AdditionalInfo;
	TArray<FName> Tags;

	bool bIsFrozen;
	FTransform Transform;
	TSubclassOf<AActor> Class;
};

USTRUCT(BlueprintType)
struct FRoomChunk
{
	GENERATED_BODY()

	FRoomChunk(): UpAngle(0), DownAngle(0), RightAngle(0), LeftAngle(0)
	{
	}

	FRoomChunk(const FVector2D ChunkLocalCoords) : ChunkLocalCoords(ChunkLocalCoords), UpAngle(0), DownAngle(0), RightAngle(0), LeftAngle(0)
	{
	}

	/*
	 * Room Chunks coordinates:
	 *			+X
	 * -Y					+Y
	 *			-X
	 * The first element is the root chunk and should be equal to 0, 0
	 * In a common room there will be only one chunk with coordinates 0, 0
	 */
	UPROPERTY(BlueprintReadOnly)
	FVector2D ChunkLocalCoords = FVector2D(0.0f, 0.0f);
	/*1000 if dead end*/
	UPROPERTY(BlueprintReadOnly)
	float UpAngle;
	/*1000 if dead end*/
	UPROPERTY(BlueprintReadOnly)
	float DownAngle;
	/*1000 if dead end*/
	UPROPERTY(BlueprintReadOnly)
	float RightAngle;
	/*1000 if dead end*/
	UPROPERTY(BlueprintReadOnly)
	float LeftAngle;
};

USTRUCT(BlueprintType)
struct FRoomState
{
	GENERATED_BODY()

	FRoomState(): DistanceMesh(nullptr), Height(-1), Heat(-1), Moisture(-1)
	{
	}

	explicit FRoomState(const FVector2D Coords) : DistanceMesh(nullptr), Coords(Coords), Height(-1), Heat(-1), Moisture(-1)
	{
	}

	UPROPERTY(BlueprintReadOnly)
	TSoftObjectPtr<UWorld> Level;
	UPROPERTY(BlueprintReadOnly)
	UStaticMesh* DistanceMesh;

	UPROPERTY(BlueprintReadOnly)
	FName BiomeName;
	// Real coordinates. NOT GRID COORDINATES
	UPROPERTY(BlueprintReadOnly)
	FVector2D Coords;
	UPROPERTY(BlueprintReadOnly)
	FVector Offset;
	UPROPERTY(BlueprintReadOnly)
	FRotator Rotation;
	UPROPERTY(BlueprintReadOnly)
	TArray<FRoomChunk> Chunks;

	float Height;
	float Heat;
	float Moisture;

	TMap<int32, FObjectState> StaticObjects;

	bool bShouldNotBeDeletedByChunks = false;
	bool bIsFlat = false;
};

UCLASS()
class MAZEGAME_API UMGMazeStateSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TMap<int32, FRoomState> RoomsStates;
	TMap<int32, FDynamicObjectState> DynamicObjects;
	//TODO
	TMap<int32, FObjectState> AIObjects;
	//----

	UPROPERTY(BlueprintReadOnly)
	float GridSize;

	UFUNCTION(BlueprintPure)
	static int32 GetRoomObjectHash(AActor* Actor);

	UFUNCTION(BlueprintPure, meta = (WorldContext = WorldContextObject))
	static int32 GetRoomHash(UObject* WorldContextObject, float X, float Y);

	bool TryToSpawn(FRoomInfo RoomToSpawn, double Angle, TArray<TPair<int32, FRoomState>>* BiomeRoomsToRemoveFrom, int32 RootIndexInBiomeRooms, bool bCanOverride = true);

private:
	static TArray<FVector2D> GetRotatedChunksArray(TArray<FVector2D>& ChunksToRotate, double Angle);
};
