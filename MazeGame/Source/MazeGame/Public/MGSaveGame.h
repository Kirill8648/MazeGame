// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MGGameplayAbilityBase.h"
#include "GameFramework/SaveGame.h"
#include "MGSaveGame.generated.h"

/** 
 * Структура, хранящая текущий уровень способностей.
 */
USTRUCT(BlueprintType)
struct FSavedAbilityInfo
{
	GENERATED_BODY()
	/**
	* Стандартный конструктор.
	*/
	FSavedAbilityInfo()
	{
		AbilityInfo = NULL;
	}

	/**
	* Конструктор с параметрами.
	* @param AbilityKey - ключ способности
	* @param AbilityInfo - уровень способности
	*/
	FSavedAbilityInfo(FString AbilityKey, int32 AbilityInfo): AbilityKey(AbilityKey), AbilityInfo(AbilityInfo)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AbilityKey; /**< Ключ способности. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AbilityInfo; /**< Уровень способности. */
};

/** 
 * Структура, хранящая данные об одной способности.
 */
USTRUCT(BlueprintType)
struct FAbilityLevelPrices : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AbilityKey = ""; /**< Ключ способности. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl1Cost = 0; /**< Цена улучшения способности на 1 уровень. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl2Cost = 0; /**< Цена улучшения способности на 2 уровень. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl3Cost = 0; /**< Цена улучшения способности на 3 уровень. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl4Cost = 0; /**< Цена улучшения способности на 4 уровень. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl5Cost = 0; /**< Цена улучшения способности на 5 уровень. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl6Cost = 0; /**< Цена улучшения способности на 6 уровень. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl7Cost = 0; /**< Цена улучшения способности на 7 уровень. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl8Cost = 0; /**< Цена улучшения способности на 8 уровень. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl9Cost = 0; /**< Цена улучшения способности на 9 уровень. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl10Cost = 0; /**< Цена улучшения способности на 10 уровень. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxLvl = 0; /**< максимальный уровень способности. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> AbilityIcon = nullptr; /**< Иконка способности. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMGGameplayAbilityBase> Ability; /**< Класс способности. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AbilityStatsTable = nullptr; /**< Таблица характеристик способности. */
};

/** 
 * Структура, хранящая данные об одной характеристике способности.
 */
USTRUCT(BlueprintType)
struct FAbilityStatsByLevels : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString StatKey = ""; /**< Ключ характеристики. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Lvl1Value = 0; /**< Значение характеристики на 1 уровне. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Lvl2Value = 0; /**< Значение характеристики на 2 уровне. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Lvl3Value = 0; /**< Значение характеристики на 3 уровне. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Lvl4Value = 0; /**< Значение характеристики на 4 уровне. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Lvl5Value = 0; /**< Значение характеристики на 5 уровне. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Lvl6Value = 0; /**< Значение характеристики на 6 уровне. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Lvl7Value = 0; /**< Значение характеристики на 7 уровне. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Lvl8Value = 0; /**< Значение характеристики на 8 уровне. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Lvl9Value = 0; /**< Значение характеристики на 9 уровне. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Lvl10Value = 0; /**< Значение характеристики на 10 уровне. */
};

/**
 * Класс для хранения сохраняющихся данных.
 */
UCLASS()
class MAZEGAME_API UMGSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	/**
	* Стандартный конструктор.
	*/
	UMGSaveGame();

	/**
	* Инициализирует дату и время.
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|SaveGame")
	void SetDateTime();

	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	int32 PlayerCurrentCoinsNumber; /**< Количество монет у игрока. */

	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	int32 PlayerAccumulatedCoinsNumber; /**< Количество потраченных монет у игрока. */

	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	TArray<FSavedAbilityInfo> AbilitiesLevels; /**< Уровни способностей для текущего сохранения. */

	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	TArray<FSavedAbilityInfo> AbilitiesSlotsNumbers; /**< Слоты способностей для текущего сохранения. */

	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	int32 LastXMazeSize; /**< Последняя настроенная игроком ширина лабиринта. */

	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	int32 LastYMazeSize; /**< Последняя настроенная игроком длина лабиринта. */

	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	TArray<FSavedAbilityInfo> SpawnedAbilityCollectiblesCache; /**< Кэш заспавненных ключей способностей, использующийся для избежания их повторения. */

	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	int32 SlotIndex; /**< Мета индекс слота сохранения. */
	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	FString Date; /**< Мета дата слота сохранения. */
	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	FString Time; /**< Мета время слота сохранения. */
};
