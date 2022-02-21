// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGMazeGenerator.generated.h"

enum EMazeItemState
{
	None,
	Cell,
	VerticalEdge,
	HorizontalEdge,
	Knot,
};

struct MazeItem
{
	EMazeItemState MazeItemState = None;
	int32 Distance = 0;
	bool bIsUnbreakable = false;
	bool bIsVisited = false;
	int32 IndexY;
	int32 IndexX;
};

struct Pair
{
	int32 Y;
	int32 X;
};

UCLASS()
class MAZEGAME_API AMGMazeGenerator : public AActor
{
	GENERATED_BODY()

public:
	AMGMazeGenerator();

	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGMazeGenerator")
	void GenerateMazeWithSeed(int32 Seed, int32 XSize, int32 YSize);

	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGMazeGenerator")
	void SimpleSpawnGeneratedMaze();

	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGMazeGenerator")
	void SpawnWithInstances();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|SimpleSpawn")
	TSubclassOf<AActor> CommonMazeWall;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|SimpleSpawn")
	TSubclassOf<AActor> UnbreakableMazeWall;


	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|SpawnWithInstances")
	TSubclassOf<class AMGInstancedMeshActor> InstancedMeshCommonWalls;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|SpawnWithInstances")
	TSubclassOf<AMGInstancedMeshActor> InstancedMeshUnbreakableWalls;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MazeGame|MGMazeGenerator")
	int32 MGSeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MazeGame|MGMazeGenerator")
	int32 MGXSize;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MazeGame|MGMazeGenerator")
	int32 MGYSize;


protected:
	TArray<TArray<MazeItem>> MazeMatrix;

	virtual void BeginPlay() override;

private:
	void PrintMazeMatrixToLog();

	TArray<MazeItem*> GetNearCellsByIndexes(int32 IndexY, int32 IndexX, bool bIgnoreWalls = false);

	void PlaceWallBetweenCellsByIndexes(int32 IndexY1, int32 IndexX1, int32 IndexY2, int32 IndexX2);

	void BreakWallBetweenCellsByIndexes(int32 IndexY1, int32 IndexX1, int32 IndexY2, int32 IndexX2);

	void ShuffleArrayByStream(TArray<MazeItem*>& ArrayToShuffle, FRandomStream& Stream);

	void ShuffleArrayByStream(TArray<TArray<MazeItem*>>& ArrayToShuffle, FRandomStream& Stream);

	bool bIsMistakeHappened = false;
};
