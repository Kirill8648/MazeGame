// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MGPlayerDataSubsystem.h"
#include "GameFramework/SaveGame.h"
#include "MGSaveGame.generated.h"


/**
 * 
 */
UCLASS()
class MAZEGAME_API UMGSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UMGSaveGame()
	{
	};

	UPROPERTY(VisibleAnywhere, Category = "MazeGame|SaveGame")
	int32 PlayerCoinsNumber;

	UPROPERTY(VisibleAnywhere, Category = "MazeGame|SaveGame")
	TArray<FSavedAbilityInfo> AbilitiesArray;

	UPROPERTY(VisibleAnywhere, Category = "MazeGame|SaveGame")
	int32 LastXMazeSize;

	UPROPERTY(VisibleAnywhere, Category = "MazeGame|SaveGame")
	int32 LastYMazeSize;
};
