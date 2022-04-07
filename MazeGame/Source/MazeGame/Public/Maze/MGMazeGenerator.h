// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UI/MGMazeGenerationProgressWidget.h"
#include "MGMazeGenerator.generated.h"

DECLARE_DELEGATE_OneParam(FBoolDelegate, bool)
DECLARE_DELEGATE_OneParam(FStringDelegate, FString)

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

	FBoolDelegate MatrixGenerationFinishedDelegate;
	FStringDelegate DrawGenerationProgressUIDelegate;

	
	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGMazeGenerator")
	void LaunchAsyncMazeGeneration(int32 Seed, int32 XSize, int32 YSize);
	
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MazeGame|Parameters")
	int32 MGSeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MazeGame|Parameters")
	int32 MGXSize;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MazeGame|Parameters")
	int32 MGYSize;

	UPROPERTY(EditAnywhere, Category = "MazeGame|Widgets")
	UMGMazeGenerationProgressWidget* MazeGenerationProgressWidgetRef;

protected:
	TArray<TArray<MazeItem>> MazeMatrix;

	virtual void BeginPlay() override;

private:
	void PrintMazeMatrixToLog();

	void ContinueGeneration(bool bIsMistakeHappenedInAsync);

	void DrawGenerationProgressUI(FString StringToDisplay);

	bool bIsMistakeHappened = false;
};


class FGenerateMazeMatrixAsyncTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FGenerateMazeMatrixAsyncTask>;

public:
	FGenerateMazeMatrixAsyncTask(FBoolDelegate& EndDelegate, FStringDelegate& DrawUIDelegate, TArray<TArray<MazeItem>>& MatrixAddress, int32 Seed, int32 YSize, int32 XSize) :
		EndDelegate(EndDelegate), DrawUIDelegate(DrawUIDelegate), MatrixAddress(MatrixAddress), Seed(Seed), YSize(YSize), XSize(XSize)
	{
	}

protected:
	FBoolDelegate& EndDelegate;
	FStringDelegate& DrawUIDelegate;
	TArray<TArray<MazeItem>>& MatrixAddress;
	int32 Seed;
	int32 YSize;
	int32 XSize;
	bool bIsMistakeHappenedInAsync = false;

	void DoWork();

	TArray<MazeItem*> GetNearCellsByIndexes(int32 IndexY, int32 IndexX, bool bIgnoreWalls = false);

	void PlaceWallBetweenCellsByIndexes(int32 IndexY1, int32 IndexX1, int32 IndexY2, int32 IndexX2);

	void BreakWallBetweenCellsByIndexes(int32 IndexY1, int32 IndexX1, int32 IndexY2, int32 IndexX2);

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FMyTaskName, STATGROUP_ThreadPoolAsyncTasks);
	}
};
