// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MGInstancedCollectibleActor.h"
#include "MGInstancedMeshActor.h"
#include "MGMazeGenerationGameMode.h"
#include "GameFramework/Actor.h"
#include "Templates/Tuple.h"
#include "UI/MGMazeGenerationProgressWidget.h"
#include "MGMazeGenerator.generated.h"

class UMGBiomeData;
DECLARE_DELEGATE_OneParam(FBoolDelegate, bool)
DECLARE_DELEGATE_OneParam(FStringDelegate, FString)

/** 
 * Перечисление, означающее состояние клетки лабиринта.
 */
UENUM()
enum EMazeItemState
{
	None,
	/**< Означает что клекта пуста. */
	Cell,
	/**< Означает что клекта заполнена. */
	VerticalEdge,
	/**< Означает что клекта является вертикальной стеной. */
	HorizontalEdge,
	/**< Означает что клекта является горизонтальной стеной. */
	Knot,
	/**< Означает что клекта является узлом. */
};

/** 
 * Структура, хранящая данные о клетке лабиринта.
 */
struct FMazeItem
{
	/**
	* Стандартный конструктор.
	*/
	FMazeItem()
	{
		MazeItemState = None;
		Distance = 0;
		bIsUnbreakable = false;
		bIsVisited = false;
		IndexY = 0;
		IndexX = 0;
	}

	/**
	* Конструктор с параметрами.
	* @param IndexY - индекс клетки по Y
	* @param IndexX - индекс клетки по X
	*/
	FMazeItem(const int32 IndexY, const int32 IndexX): IndexY(IndexY), IndexX(IndexX)
	{
	}

	EMazeItemState MazeItemState = None; /**< Состояние клетки. */
	int32 Distance = 0; /**< Дистанция от клетки до выхода. */
	bool bIsUnbreakable = false; /**< Является ли клетка неразрушимой. */
	bool bIsVisited = false; /**< Посетил ли алгоритм генерации клетку. */
	int32 IndexY; /**< Индекс клетки по Y. */
	int32 IndexX; /**< Индекс клетки по X. */
};

/** 
 * Простая структура пары.
 */
struct FPair
{
	/**
	* Стандартный конструктор.
	*/
	FPair(): Y(0), X(0)
	{
	}

	/**
	* Конструктор с параметрами.
	* @param Y1 - значение Y
	* @param X1 - значение X
	*/
	FPair(const int32 Y1, const int32 X1): Y(Y1), X(X1)
	{
	}

	int32 Y; /**< Значение Y. */
	int32 X; /**< Значение X. */
};

/** 
 * Структура, хранящая данные об отдельном акторе для спавна после генерации лабиринта.
 */
USTRUCT()
struct FSeparateSpawnedActorInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SpawnedActor; /**< Класс актора для спавна. */
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EMazeItemState> ActorType;
	/**< Тип актора для спавна. Выбирать None запрещено. Knot пока не реализован. */
	UPROPERTY(EditAnywhere, meta = (UIMin = "0.0"))
	float SpawnRate;
	/**< Вероятность спавна в процентах (50% = 0.5). Означает процент от количества клеток определенного выбранного типа. */
	UPROPERTY(EditAnywhere)
	bool bIsSpawnRateAsCount;
	/**< Использовать ли вероятность спавна как количество заспавненных акторов независимо от размера лабиринта. В этом случае значение SpawnRate должно быть целое. */
	UPROPERTY(EditAnywhere)
	FVector OptionalOffset; /**< Опциональная корректировка положения актора при спавне. */
	UPROPERTY(EditAnywhere)
	FRotator OptionalRotation; /**< Опциональная корректировка поворота актора при спавне. */
};

/** 
 * Структура, хранящая данные об акторе стены с компонентом экземпляра меша для спавна.
 */
USTRUCT()
struct FInstancedWallInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TSubclassOf<AMGInstancedMeshActorStatic> InstancedActor; /**< Класс актора стены с компонентом экземпляра меша. */
	UPROPERTY(EditAnywhere)
	float HorizontalSpawnRate; /**< Вероятность спавна в процентах (50% = 0.5) на месте горизонтальной стены. */
	UPROPERTY(EditAnywhere)
	float VerticalSpawnRate; /**< Вероятность спавна в процентах (50% = 0.5) на месте вертикальной стены. */
	UPROPERTY(EditAnywhere)
	FVector OptionalOffset; /**< Опциональная корректировка положения актора при спавне. */
};

/** 
 * Структура, хранящая данные об акторе собираемого предмета с компонентом экземпляра меша для спавна.
 */
USTRUCT()
struct FInstancedCollectibleInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TSubclassOf<AMGInstancedMeshActorStatic> InstancedActor;
	/**< Класс актора собираемого предмета с компонентом экземпляра меша. */
	UPROPERTY(EditAnywhere)
	float SpawnRate; /**< Вероятность спавна в процентах (50% = 0.5) на месте клетки типа Cell. */
	UPROPERTY(EditAnywhere)
	FVector OptionalOffset; /**< Опциональная корректировка положения актора при спавне. */
};

/** 
 * Структура, хранящая данные об акторе пола для спавна.
 */
USTRUCT()
struct FSeparateSpawnedFloorActorInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> SpawnedFloorActor; /**< Класс актора пола для спавна. */
	UPROPERTY(EditAnywhere, meta = (UIMin = "0.0"))
	float SpawnRate; /**< Вероятность спавна в процентах (50% = 0.5). Означает процент от количества клеток пола. */
	UPROPERTY(EditAnywhere)
	bool bIsSpawnRateAsCount;
	/**< Использовать ли вероятность спавна как количество заспавненных акторов независимо от размера лабиринта. В этом случае значение SpawnRate должно быть целое. */
	UPROPERTY(EditAnywhere)
	FVector OptionalOffset; /**< Опциональная корректировка положения актора при спавне. */
	UPROPERTY(EditAnywhere)
	FRotator OptionalRotation; /**< Опциональная корректировка поворота актора при спавне. */
};

/** 
 * Генератор лабиринта.
 */
UCLASS()
class MAZEGAME_API AMGMazeGenerator : public AActor
{
	GENERATED_BODY()

public:
	/**
	* Стандартный конструктор.
	*/
	AMGMazeGenerator();
	/**
	* Запускает асинхронную генерацию лабиринта.
	* @param Seed - ключ генерации
	* @param XSize - значение X
	* @param YSize - значение Y
	* @param ChangeLoadingScreenTextDelegate - делегат, вызывающийся для смены текста загрузки
	* @param AllFinishedDelegate - делегат, вызывающийся при завершении генерации
	*/
	void LaunchAsyncMazeGeneration(int32 Seed, int32 XSize, int32 YSize,
	                               FString2Delegate& ChangeLoadingScreenTextDelegate, FDelegate& AllFinishedDelegate);
	/**
	* Возвращает координаты для старта игрока.
	* @return координаты для старта игрока.
	*/
	FVector GetPlayerStartCoords();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|UnbreakableWalls")
	TSubclassOf<AMGInstancedMeshActorStatic> InstancedMeshUnbreakableWalls;
	/**< Класс актора неразрушаемой стены с компонентом экземпляра меша. */

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MazeGame|Parameters")
	int32 DistanceBetweenWalls; /**< Расстояние между стенами в лабиринте. */

	UPROPERTY(EditAnywhere, Category = "MazeGame|Widgets")
	TSubclassOf<UMGMazeGenerationProgressWidget> MazeGenerationProgressWidget; /**<Класс виджета генерации лабиринта. */

	UPROPERTY(BlueprintReadOnly, Category = "MazeGame|Widgets")
	UMGMazeGenerationProgressWidget* MazeGenerationProgressWidgetRef; /**< Укзатель на виджет генерации лабиринта. */

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|Collectibles")
	TSubclassOf<AMGInstancedCollectibleActor> InstancedMeshCoins;
	/**< Класс актора валюты с компонентом экземпляра меша. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|Collectibles")
	FVector OptionalInstancedMeshCoinsOffset;
	/**< Опциональная корректировка положения актора валюты с компонентом экземпляра меша при спавне. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|Collectibles")
	TSubclassOf<AActor> AbilityCollectibleActor; /**< Класс актора собираемой способности. */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|Collectibles")
	int32 NumOfSpawnedAbilityCollectibleActor; /**< Количество акторов собираемой способности на весь лабиринт. */

	UPROPERTY(EditAnywhere, Category = "MazeGame|SpawnedObjects")
	TSubclassOf<AActor> ExitLevelVolume; /**< Класс триггера завершения уровня. */
	UPROPERTY(EditAnywhere, Category = "MazeGame|SpawnedObjects")
	TArray<FSeparateSpawnedActorInfo> SeparateActors;
	/**< Массив акторов которые будут заспавнены используя процент (SpawnRate). */
	UPROPERTY(EditAnywhere, Category = "MazeGame|SpawnedObjects")
	TArray<FInstancedWallInfo> InstancedWallsVariations; /**< Массив вариаций мешей стен. */
	UPROPERTY(EditAnywhere, Category = "MazeGame|SpawnedObjects")
	TArray<FInstancedCollectibleInfo> InstancedCollectibles; /**< Массив собираемых предметов, ИСКЛЮЧАЯ валюту. */
	UPROPERTY(EditAnywhere, Category = "MazeGame|SpawnedObjects")
	float CoinsSpawnRate; /**< Процент спавна монет. */

	UPROPERTY(EditAnywhere, Category = "MazeGame|Floor")
	TArray<FInstancedCollectibleInfo> InstancedFloorVariations; /**< Массив вариаций мешей пола. */
	UPROPERTY(EditAnywhere, Category = "MazeGame|Floor")
	TArray<FSeparateSpawnedFloorActorInfo> FloorActors; /**< Массив акторов пола. */

	/*Height Redistribution curve over time from 0 to 1*/
	UPROPERTY(EditAnywhere, Category = "MazeGame|Biomes")
	UCurveFloat* HeightRedistribution;
	/*
	 * Height Octaves in the following format:
	 * FVector2D (InX: OctaveWeight, InY: OctaveValue) */
	UPROPERTY(EditAnywhere, Category = "MazeGame|Biomes")
	TArray<FVector2D> HeightOctaves = {FVector2D(1.0f, 0.01f), FVector2D(0.5f, 0.02f), FVector2D(0.25f, 0.04f)};

	/*Heat Redistribution curve over time from 0 to 1*/
	UPROPERTY(EditAnywhere, Category = "MazeGame|Biomes")
	UCurveFloat* HeatRedistribution;
	/*
	 * Heat Octaves in the following format:
	 * FVector2D (InX: OctaveWeight, InY: OctaveValue) */
	UPROPERTY(EditAnywhere, Category = "MazeGame|Biomes")
	TArray<FVector2D> HeatOctaves = {FVector2D(1.0f, 0.02f), FVector2D(0.5f, 0.04f), FVector2D(0.25f, 0.08f)};

	/*Moisture Redistribution curve over time from 0 to 1*/
	UPROPERTY(EditAnywhere, Category = "MazeGame|Biomes")
	UCurveFloat* MoistureRedistribution;
	/*
	 * Moisture Octaves in the following format:
	 * FVector2D (InX: OctaveWeight, InY: OctaveValue) */
	UPROPERTY(EditAnywhere, Category = "MazeGame|Biomes")
	TArray<FVector2D> MoistureOctaves = {FVector2D(2.0f, 0.02f), FVector2D(0.25f, 0.04f), FVector2D(0.125f, 0.06f)};

	UPROPERTY(EditAnywhere, Category = "MazeGame|Biomes")
	TArray<UMGBiomeData*> Biomes;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MazeGame")
	int32 Seed;
	
protected:
	TArray<TArray<FMazeItem>> MazeMatrix; /**< Массив, представляющий генерируемый лабиринт. */
	/**
	* Функция, вызывающаяся в момент начала игры.
	*/
	virtual void BeginPlay() override;

private:
	FBoolDelegate MatrixGenerationFinishedDelegate;
	/**< Делегат, вызывающийся при завершении генерации матрицы лабиринта. */
	FStringDelegate DrawGenerationProgressUIDelegate;
	/**< Делегат, вызывающийся при необходимости перерисовать процесс генерации на виджете. */
	bool bIsMistakeHappened = false; /**< Случились ли ошибки при генерации. */
	FMazeItem ExitCell; /**< Данные о клетке с выходом из лабиринта. */

	FString2Delegate ChangeLoadingScreenTextDelegate; /**< Делегат, вызывающийся для смены текста загрузки. */
	FDelegate AllFinishedDelegate; /**<Делегат, вызывающийся при завершении генерации.*/
	/**
	* Напечатать матрицу в лог.
	*/
	void PrintMazeMatrixToLog();
	/**
	* Продолжить генерацию после генерации матрицы (расставить объекты).
	* @param bIsMistakeHappenedInAsync - случились ли ошибки при выполнении асинхронной задачи
	*/
	void ContinueGeneration(bool bIsMistakeHappenedInAsync);
	/**
	* Перерисовать процесс генерации на виджете.
	* @param StringToDisplay - какую строку отрисовать
	*/
	void DrawGenerationProgressUI(FString StringToDisplay);
	/**
	* Заспавнить объекты.
	*/
	void SpawnObjects();
	/**
	* Перемешать массив.
	* @param Array - адрес перемешиваемого массива
	* @param Stream - адрес потока рандомных чисел
	*/
	template <typename InElementType, typename InAllocatorType>
	static void ShuffleArray(TArray<InElementType, InAllocatorType>& Array, FRandomStream& Stream);
	/*UPROPERTY(EditAnywhere, Category = "MazeGame")
	int32 Seed; /**<Ключ генерации.*/

	UFUNCTION(BlueprintCallable)
	TArray<UTexture2D*> GenerateBiomes();

	void PerlinReset(FRandomStream& Stream);
	float PerlinNoise2D(const FVector2D& Location);
};

/** 
 * Асинхронная задача для генерации матрицы лабиринта
 */
class FGenerateMazeMatrixAsyncTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FGenerateMazeMatrixAsyncTask>;

public:
	/**
	* Конструктор с параметрами.
	* @param EndDelegate - адрес делегата, вызваемого при завершении генерации матрицы
	* @param DrawUIDelegate - адрес делегата, вызваемого при необходимости перерисовать процесс генерации
	* @param MatrixAddress - адрес матрицы лабиринта
	* @param ExitCell - адрес клетки с выходом
	* @param Seed - ключ генерации
	* @param YSize - размер Y
	* @param XSize - размер X
	*/
	FGenerateMazeMatrixAsyncTask(FBoolDelegate& EndDelegate, FStringDelegate& DrawUIDelegate,
	                             TArray<TArray<FMazeItem>>& MatrixAddress, FMazeItem& ExitCell, int32 Seed,
	                             int32 YSize, int32 XSize) : EndDelegate(EndDelegate), DrawUIDelegate(DrawUIDelegate),
	                                                         MatrixAddress(MatrixAddress), ExitCell(ExitCell),
	                                                         Seed(Seed), YSize(YSize), XSize(XSize)
	{
	}

protected:
	FBoolDelegate& EndDelegate; /**<Адрес делегата, вызваемого при завершении генерации матрицы.*/
	FStringDelegate& DrawUIDelegate; /**< Адрес делегата, вызваемого при необходимости перерисовать процесс генерации.*/
	TArray<TArray<FMazeItem>>& MatrixAddress; /**<Адрес матрицы лабиринта.*/
	FMazeItem& ExitCell; /**<Адрес клетки с выходом.*/
	int32 Seed; /**<Ключ генерации.*/
	int32 YSize; /**<Размер Y.*/
	int32 XSize; /**<Размер X.*/
	bool bIsMistakeHappenedInAsync = false; /**<Случились ли ошибки при асинхронной генерации матрицы лабиринта.*/
	/**
	* Сделать работу (асинхронно сгенерировать матрицу лабиринта).
	*/
	void DoWork();
	/**
	* Получить указатели на соседние клетки.
	* @param IndexY - индекс клетки по Y
	* @param IndexX - индекс клетки по X
	* @param bIgnoreWalls - игнорировать ли стены
	* @return массив указателей на клетки
	*/
	TArray<FMazeItem*> GetNearCellsByIndexes(int32 IndexY, int32 IndexX, bool bIgnoreWalls = false);
	/**
	* Поставить стену между клетками по индексам.
	* @param IndexY1 - индекс клетки 1 по Y
	* @param IndexX1 - индекс клетки 1 по X
	* @param IndexY2 - индекс клетки 2 по Y
	* @param IndexX2 - индекс клетки 2 по X
	*/
	void PlaceWallBetweenCellsByIndexes(int32 IndexY1, int32 IndexX1, int32 IndexY2, int32 IndexX2);
	/**
	* Разрушить стену между клетками по индексам.
	* @param IndexY1 - индекс клетки 1 по Y
	* @param IndexX1 - индекс клетки 1 по X
	* @param IndexY2 - индекс клетки 2 по Y
	* @param IndexX2 - индекс клетки 2 по X
	*/
	void BreakWallBetweenCellsByIndexes(int32 IndexY1, int32 IndexX1, int32 IndexY2, int32 IndexX2);
	/**
	* Техническая функция, необходимая для реализации FNonAbandonableTask.
	*/
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FMyTaskName, STATGROUP_ThreadPoolAsyncTasks);
	}
};
