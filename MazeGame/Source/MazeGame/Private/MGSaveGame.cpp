// Fill out your copyright notice in the Description page of Project Settings.


#include "MGSaveGame.h"

#include "MazeGame/MazeGame.h"

UMGSaveGame::UMGSaveGame()
{
	PlayerCurrentCoinsNumber = 0;
	PlayerAccumulatedCoinsNumber = 0;
	LastXMazeSize = 100;
	LastYMazeSize = 100;
	SetDateTime();

	if (static ConstructorHelpers::FObjectFinder<UDataTable> AbilitiesTable(TEXT("/Game/MazeGame/Data/DataTables/DT_Abilities.DT_Abilities")); AbilitiesTable.Succeeded())
	{
		TArray<FAbilityLevelPrices*> Rows;
		AbilitiesTable.Object->GetAllRows("Error trying to GetAllRows() in MGSaveGame.cpp", Rows);
		for (const FAbilityLevelPrices* Row : Rows)
			if (Row->AbilityKey.Equals("RaiseWalls"))
				AbilitiesLevels.Emplace(FSavedAbilityInfo("RaiseWalls", 1));
			else
				//TEMP
				AbilitiesLevels.Emplace(FSavedAbilityInfo(Row->AbilityKey, 0));
	}

	AbilitiesSlotsNumbers.Emplace(FSavedAbilityInfo("RaiseWalls", 0));
	for (int i = 1; i < 5; i++)
		AbilitiesSlotsNumbers.Emplace(FSavedAbilityInfo("", i));
}

void UMGSaveGame::SetDateTime()
{
	const FDateTime DateTime = FDateTime::Now();
	Date = FString::Printf(TEXT("%02d.%02d.%d"), DateTime.GetDay(), DateTime.GetMonth(), DateTime.GetYear());
	Time = FString::Printf(TEXT("%02d:%02d"), DateTime.GetHour(), DateTime.GetMinute());
}
