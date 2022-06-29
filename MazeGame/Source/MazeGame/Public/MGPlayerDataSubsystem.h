// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MGSaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MGPlayerDataSubsystem.generated.h"

/**
 * Класс подсистемы для хранения данных о текущей игровой сессии.
 */
UCLASS()
class MAZEGAME_API UMGPlayerDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	* Стандартный конструктор.
	*/
	UMGPlayerDataSubsystem();

	/**
	* Создает новый файл сохранения.
	* @param SlotIndex - индекс слота сохранения
	* @param bIsOverriding - допускает ли перезапись если слот сохранения с данным индексом уже существует
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|PlayerSubsystem")
	void CreateNewSaveFileAndSave(int32 SlotIndex, bool bIsOverriding);

	/**
	* Удалаяет файл сохранения.
	* @param SlotIndex - индекс слота сохранения
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|PlayerSubsystem")
	void DeleteSaveFileBySlot(int32 SlotIndex);

	/**
	* Выгружает объект класса сохранения из подсистемы.
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|PlayerSubsystem")
	void UnloadGameFromPlayerDataSubsystem();

	/**
	* Асинхронно загружает сохранение из слота.
	* @param SlotIndex - индекс слота сохранения
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|PlayerSubsystem")
	void LoadGameFromSlotAsync(int32 SlotIndex);

	/**
	* Асинхронно сохраняет файл сохранения в слот.
	* @param SlotIndex - индекс слота сохранения
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|PlayerSubsystem")
	void SaveGameToSlotAsync(int32 SlotIndex);

	/**
	* Загружает файл сохранения из слота.
	* @param SlotIndex - индекс слота сохранения
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|PlayerSubsystem")
	void LoadGameFromSlot(int32 SlotIndex);

	/**
	* Сохраняет файл сохранения в слот.
	* @param SlotIndex - индекс слота сохранения
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|PlayerSubsystem")
	void SaveGameToSlot(int32 SlotIndex);

	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|PlayerSubsystem")
	int32 CurrentRunSeed = 0; /**< Последний введенный игроком ключ генерации. */

	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|PlayerSubsystem")
	UMGSaveGame* CurrentlyLoadedSaveGameObject; /**< Указатель на объект текущего загруженного сохранения. */

	UPROPERTY(BlueprintReadOnly, Category = "MazeGame|PlayerSubsystem")
	UDataTable* AbilitiesTable; /**< Таблица с информацией о способностях. */

	float CurrentMazeCellSize = 500.0f;

	UPROPERTY(BlueprintReadOnly)
	mutable bool bRealTimePortals = false;
	UPROPERTY(BlueprintReadOnly)
	mutable float PortalsUpdateRate = 1.0f;

	//TSoftClassPtr<UDataTable> AbilitiesTableSoftClassPtr;

	//TSoftObjectPtr<UDataTable> AbilitiesTableSoftObjectPtr;
private:
	/**
	* Вызывается по завершении асинхронной загрузки сохранения.
	* @param SlotName - имя слота
	* @param UserIndex - индекс игрока
	* @param LoadedGameData - указатель объект сохранения 
	*/
	void LoadGameDelegateFunction(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);

	/**
	* Вызывается по завершении асинхронного сохранения.
	* @param SlotName - имя слота
	* @param UserIndex - индекс игрока
	* @param bSuccess - успешно ли прошло сохранение
	*/
	void SavedCreatedFileDelegateFunction(const FString& SlotName, const int32 UserIndex, bool bSuccess);
};
