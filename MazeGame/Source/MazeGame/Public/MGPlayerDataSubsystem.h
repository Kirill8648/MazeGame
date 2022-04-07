// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MGGameplayAbilityBase.h"
#include "Engine/DataTable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MGPlayerDataSubsystem.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FAbilityLevelPrices : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AbilityName;
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
	TSoftObjectPtr<UTexture> AbilityIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UMGGameplayAbilityBase> Ability;
};

USTRUCT()
struct FSavedAbilityInfo
{
	GENERATED_BODY()
	FString AbilityName;
	int32 AbilityLevel;
};

UCLASS()
class MAZEGAME_API UMGPlayerDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMGPlayerDataSubsystem();

	UFUNCTION(BlueprintCallable, Category = "MazeGame|PlayerSubsystem")
	void LoadGameFromSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "MazeGame|PlayerSubsystem")
	void SaveGameToSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "MazeGame|PlayerSubsystem")
	void SaveGameToLastSlot();

private:
	int32 LastSlotIndex;
};
