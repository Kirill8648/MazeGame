// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MGInstancedCollectibleActor.h"
#include "MGInstancedMeshActor.h"
#include "MGMazeGenerationGameMode.h"
#include "GameFramework/Actor.h"
#include "UI/MGMazeGenerationProgressWidget.h"
#include "MGMazeGenerator.generated.h"

DECLARE_DELEGATE_OneParam(FBoolDelegate, bool)
DECLARE_DELEGATE_OneParam(FStringDelegate, FString)

UENUM()
enum EMazeItemState
{
	None,
	Cell,
	VerticalEdge,
	HorizontalEdge,
	Knot,
};

struct FMazeItem
{
	FMazeItem()
	{
		MazeItemState = None;
		Distance = 0;
		bIsUnbreakable = false;
		bIsVisited = false;
		IndexY = 0;
		IndexX = 0;
	}

	FMazeItem(const int32 IndexY, const int32 IndexX): IndexY(IndexY), IndexX(IndexX)
	{
	}

	EMazeItemState MazeItemState = None;
	int32 Distance = 0;
	bool bIsUnbreakable = false;
	bool bIsVisited = false;
	int32 IndexY;
	int32 IndexX;
};

struct FPair
{
	FPair(const int32 Y1, const int32 X1): Y(Y1), X(X1)
	{
	}

	FPair(): Y(0), X(0)
	{
	}

	int32 Y;
	int32 X;
};

USTRUCT()
struct FSeparateSpawnedActorInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SpawnedActor;
	//None not allowed
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EMazeItemState> ActorType;
	UPROPERTY(EditAnywhere, meta = (UIMin = "0.0"))
	float SpawnRate;
	//Is SpawnRate simply means the amount of actors regardless of the size of the maze
	UPROPERTY(EditAnywhere)
	bool bIsSpawnRateAsCount;
	UPROPERTY(EditAnywhere)
	FVector OptionalOffset;
	UPROPERTY(EditAnywhere)
	FRotator OptionalRotation;
};

USTRUCT()
struct FInstancedWallInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TSubclassOf<AMGInstancedMeshActorStatic> InstancedActor;
	UPROPERTY(EditAnywhere)
	float HorizontalSpawnRate;
	UPROPERTY(EditAnywhere)
	float VerticalSpawnRate;
	UPROPERTY(EditAnywhere)
	FVector OptionalOffset;
};

USTRUCT()
struct FInstancedCollectibleInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TSubclassOf<AMGInstancedMeshActorStatic> InstancedActor;
	UPROPERTY(EditAnywhere)
	float SpawnRate;
	UPROPERTY(EditAnywhere)
	FVector OptionalOffset;
};

USTRUCT()
struct FSeparateSpawnedFloorActorInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SpawnedFloorActor;
	UPROPERTY(EditAnywhere, meta = (UIMin = "0.0"))
	float SpawnRate;
	//Is SpawnRate simply means the amount of actors regardless of the size of the maze
	UPROPERTY(EditAnywhere)
	bool bIsSpawnRateAsCount;
	UPROPERTY(EditAnywhere)
	FVector OptionalOffset;
	UPROPERTY(EditAnywhere)
	FRotator OptionalRotation;
};

UCLASS()
class MAZEGAME_API AMGMazeGenerator : public AActor
{
	GENERATED_BODY()

public:
	AMGMazeGenerator();

	void LaunchAsyncMazeGeneration(int32 Seed, int32 XSize, int32 YSize, FString2Delegate& ChangeLoadingScreenTextDelegate, FDelegate& AllFinishedDelegate);

	FVector GetPlayerStartCoords();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|UnbreakableWalls")
	TSubclassOf<AMGInstancedMeshActorStatic> InstancedMeshUnbreakableWalls;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MazeGame|Parameters")
	int32 DistanceBetweenWalls;

	UPROPERTY(EditAnywhere, Category = "MazeGame|Widgets")
	TSubclassOf<UMGMazeGenerationProgressWidget> MazeGenerationProgressWidget;

	UPROPERTY(BlueprintReadOnly, Category = "MazeGame|Widgets")
	UMGMazeGenerationProgressWidget* MazeGenerationProgressWidgetRef;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|Collectibles")
	TSubclassOf<AMGInstancedCollectibleActor> InstancedMeshCoins;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|Collectibles")
	FVector OptionalInstancedMeshCoinsOffset;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|Collectibles")
	TSubclassOf<AActor> AbilityCollectibleActor;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|Collectibles")
	int32 NumOfSpawnedAbilityCollectibleActor;

	UPROPERTY(EditAnywhere, Category = "MazeGame|SpawnedObjects")
	TSubclassOf<AActor> ExitLevelVolume;
	//Actors that will be spawned as separate actors using a percentage (SpawnRate)
	UPROPERTY(EditAnywhere, Category = "MazeGame|SpawnedObjects")
	TArray<FSeparateSpawnedActorInfo> SeparateActors;
	//Different possible walls meshes
	UPROPERTY(EditAnywhere, Category = "MazeGame|SpawnedObjects")
	TArray<FInstancedWallInfo> InstancedWallsVariations;
	//Collectibles EXCLUDING coins!
	UPROPERTY(EditAnywhere, Category = "MazeGame|SpawnedObjects")
	TArray<FInstancedCollectibleInfo> InstancedCollectibles;
	UPROPERTY(EditAnywhere, Category = "MazeGame|SpawnedObjects")
	float CoinsSpawnRate;

	UPROPERTY(EditAnywhere, Category = "MazeGame|Floor")
	TArray<FInstancedCollectibleInfo> InstancedFloorVariations;
	UPROPERTY(EditAnywhere, Category = "MazeGame|Floor")
	TArray<FSeparateSpawnedFloorActorInfo> FloorActors;

protected:
	TArray<TArray<FMazeItem>> MazeMatrix;

	virtual void BeginPlay() override;

private:
	FBoolDelegate MatrixGenerationFinishedDelegate;
	FStringDelegate DrawGenerationProgressUIDelegate;
	bool bIsMistakeHappened = false;
	FMazeItem ExitCell;

	FString2Delegate ChangeLoadingScreenTextDelegate;
	FDelegate AllFinishedDelegate;

	void PrintMazeMatrixToLog();

	void ContinueGeneration(bool bIsMistakeHappenedInAsync);

	void DrawGenerationProgressUI(FString StringToDisplay);

	void SpawnObjects();
	void ShuffleArray(TArray<int32>& Array, FRandomStream& Stream);
	int32 Seed;
};


class FGenerateMazeMatrixAsyncTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FGenerateMazeMatrixAsyncTask>;

public:
	FGenerateMazeMatrixAsyncTask(FBoolDelegate& EndDelegate, FStringDelegate& DrawUIDelegate, TArray<TArray<FMazeItem>>& MatrixAddress, FMazeItem& ExitCell, int32 Seed,
	                             int32 YSize, int32 XSize) : EndDelegate(EndDelegate), DrawUIDelegate(DrawUIDelegate), MatrixAddress(MatrixAddress), ExitCell(ExitCell),
	                                                         Seed(Seed), YSize(YSize), XSize(XSize)
	{
	}

protected:
	FBoolDelegate& EndDelegate;
	FStringDelegate& DrawUIDelegate;
	TArray<TArray<FMazeItem>>& MatrixAddress;
	FMazeItem& ExitCell;
	int32 Seed;
	int32 YSize;
	int32 XSize;
	bool bIsMistakeHappenedInAsync = false;

	void DoWork();

	TArray<FMazeItem*> GetNearCellsByIndexes(int32 IndexY, int32 IndexX, bool bIgnoreWalls = false);

	void PlaceWallBetweenCellsByIndexes(int32 IndexY1, int32 IndexX1, int32 IndexY2, int32 IndexX2);

	void BreakWallBetweenCellsByIndexes(int32 IndexY1, int32 IndexX1, int32 IndexY2, int32 IndexX2);

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FMyTaskName, STATGROUP_ThreadPoolAsyncTasks);
	}
};
