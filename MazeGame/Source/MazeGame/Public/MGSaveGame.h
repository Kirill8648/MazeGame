// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MGGameplayAbilityBase.h"
#include "GameFramework/SaveGame.h"
#include "MGSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FSavedAbilityInfo
{
	GENERATED_BODY()
	FSavedAbilityInfo()
	{
		AbilityInfo = NULL;
	}

	FSavedAbilityInfo(FString AbilityKey, int32 AbilityInfo): AbilityKey(AbilityKey), AbilityInfo(AbilityInfo)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AbilityKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AbilityInfo;
};

USTRUCT(BlueprintType)
struct FAbilityLevelPrices : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AbilityKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl1Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl2Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl3Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl4Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl5Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl6Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl7Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl8Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl9Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Lvl10Cost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxLvl;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> AbilityIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UMGGameplayAbilityBase> Ability;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* AbilityStatsTable;
};

/**
 * 
 */
UCLASS()
class MAZEGAME_API UMGSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UMGSaveGame();

	UFUNCTION(BlueprintCallable, Category = "MazeGame|SaveGame")
	void SetDateTime();

	/*????????????????
	int32 MaxHealth;
	int32 MaxStamina;
	int32 MaxEnergy;*/

	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	int32 PlayerCurrentCoinsNumber;

	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	int32 PlayerAccumulatedCoinsNumber;

	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	TArray<FSavedAbilityInfo> AbilitiesLevels;

	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	TArray<FSavedAbilityInfo> AbilitiesSlotsNumbers;

	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	int32 LastXMazeSize;

	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	int32 LastYMazeSize;

	//Meta
	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	int32 SlotIndex;
	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	FString Date;
	UPROPERTY(BlueprintReadWrite, Category = "MazeGame|SaveGame")
	FString Time;
};
