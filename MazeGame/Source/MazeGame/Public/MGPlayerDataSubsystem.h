// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MGSaveGame.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MGPlayerDataSubsystem.generated.h"

/**
 * 
 */

UCLASS()
class MAZEGAME_API UMGPlayerDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMGPlayerDataSubsystem();

	UFUNCTION(BlueprintCallable, Category = "MazeGame|PlayerSubsystem")
	void CreateNewSaveFileAndSave(int32 SlotIndex, bool bIsOverriding);

	UFUNCTION(BlueprintCallable, Category = "MazeGame|PlayerSubsystem")
	void DeleteSaveFileBySlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "MazeGame|PlayerSubsystem")
	void UnloadGameFromPlayerDataSubsystem();

	UFUNCTION(BlueprintCallable, Category = "MazeGame|PlayerSubsystem")
	void LoadGameFromSlotAsync(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "MazeGame|PlayerSubsystem")
	void SaveGameToSlotAsync(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "MazeGame|PlayerSubsystem")
	void LoadGameFromSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "MazeGame|PlayerSubsystem")
	void SaveGameToSlot(int32 SlotIndex);

	/*UFUNCTION(BlueprintCallable, Category = "MazeGame|PlayerSubsystem")
	void SaveGameToLastSlot();*/

	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|PlayerSubsystem")
	int32 CurrentRunSeed;

	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|PlayerSubsystem")
	UMGSaveGame* CurrentlyLoadedSaveGameObject;

	UPROPERTY(BlueprintReadOnly, Category = "MazeGame|PlayerSubsystem")
	UDataTable* AbilitiesTable;

private:
	//int32 LastSlotIndex;

	void LoadGameDelegateFunction(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);

	void SavedCreatedFileDelegateFunction(const FString& SlotName, const int32 UserIndex, bool bSuccess);
};
