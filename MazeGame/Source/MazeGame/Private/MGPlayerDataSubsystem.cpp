// Fill out your copyright notice in the Description page of Project Settings.


#include "MGPlayerDataSubsystem.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

UMGPlayerDataSubsystem::UMGPlayerDataSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> AbilitiesTable1(TEXT("DataTable'/Game/MazeGame/Data/DataTables/DT_Abilities.DT_Abilities'"));
	AbilitiesTable = AbilitiesTable1.Object;
	
	/*static ConstructorHelpers::FObjectFinder<UDataTable> Table(TEXT("/Game/MazeGame/Data/DataTables/DT_Abilities.DT_Abilities"));
	AbilitiesTable = Table.Object;
	CurrentlyLoadedSaveGameObject = nullptr;*/

	//AbilitiesTable = FindObject<UDataTable>(nullptr, TEXT("/Game/MazeGame/Data/DataTables/DT_Abilities.DT_Abilities"));
	//UDataTable* TestToRun = FindObject<UDataTable>(nullptr, TEXT("some fuckin name"));
	//AbilitiesTable = LoadObject<UDataTable>(NULL, TEXT("/Game/MazeGame/Data/DataTables/DT_Abilities.DT_Abilities"), TEXT("/Game/MazeGame/Data/DataTables/DT_Abilities.DT_Abilities"), LOAD_None, NULL);

	/*UObject* obj = FindObject<UDataTable>(ANY_PACKAGE,TEXT("/Game/MazeGame/Data/DataTables/DT_Abilities.DT_Abilities"));
	AbilitiesTable = Cast<UDataTable>(obj);*/

	/*static ConstructorHelpers::FClassFinder<UDataTable> ClassFinder(TEXT("DataTable'/Game/MazeGame/Data/DataTables/DT_Abilities.DT_Abilities'"));
	AbilitiesTableSoftClassPtr = ClassFinder.Class;*/
	//AbilitiesTable = AbilitiesTableSoftClassPtr.LoadSynchronous();
}

void UMGPlayerDataSubsystem::CreateNewSaveFileAndSave(const int32 SlotIndex, const bool bIsOverriding = false)
{
	if (!UGameplayStatics::DoesSaveGameExist(FString::Printf(TEXT("SaveGame%d"), SlotIndex), 0) || bIsOverriding)
		if (UMGSaveGame* SaveGameInstance = Cast<UMGSaveGame>(UGameplayStatics::CreateSaveGameObject(UMGSaveGame::StaticClass())))
		{
			SaveGameInstance->SlotIndex = SlotIndex;
			UGameplayStatics::SaveGameToSlot(SaveGameInstance, FString::Printf(TEXT("SaveGame%d"), SlotIndex), 0);
		}
}

void UMGPlayerDataSubsystem::DeleteSaveFileBySlot(const int32 SlotIndex)
{
	UGameplayStatics::DeleteGameInSlot(FString::Printf(TEXT("SaveGame%d"), SlotIndex), 0);
}

void UMGPlayerDataSubsystem::UnloadGameFromPlayerDataSubsystem()
{
	CurrentlyLoadedSaveGameObject = nullptr;
}

void UMGPlayerDataSubsystem::LoadGameFromSlotAsync(const int32 SlotIndex)
{
	FAsyncLoadGameFromSlotDelegate LoadedDelegate;
	LoadedDelegate.BindUObject(this, &UMGPlayerDataSubsystem::LoadGameDelegateFunction);
	UGameplayStatics::AsyncLoadGameFromSlot(FString::Printf(TEXT("SaveGame%d"), SlotIndex), 0, LoadedDelegate);
}

void UMGPlayerDataSubsystem::SaveGameToSlotAsync(const int32 SlotIndex)
{
	if (IsValid(CurrentlyLoadedSaveGameObject))
		UGameplayStatics::AsyncSaveGameToSlot(CurrentlyLoadedSaveGameObject, FString::Printf(TEXT("SaveGame%d"), SlotIndex), 0);
}

void UMGPlayerDataSubsystem::LoadGameFromSlot(const int32 SlotIndex)
{
	if (UMGSaveGame* SG = Cast<UMGSaveGame>(UGameplayStatics::LoadGameFromSlot(FString::Printf(TEXT("SaveGame%d"), SlotIndex), 0)))
	{
		CurrentlyLoadedSaveGameObject = SG;
		CurrentlyLoadedSaveGameObject->SlotIndex = SlotIndex;
	}
}

void UMGPlayerDataSubsystem::SaveGameToSlot(const int32 SlotIndex)
{
	if (IsValid(CurrentlyLoadedSaveGameObject))
		UGameplayStatics::SaveGameToSlot(CurrentlyLoadedSaveGameObject, FString::Printf(TEXT("SaveGame%d"), SlotIndex), 0);
}

/*void UMGPlayerDataSubsystem::SaveGameToLastSlot()
{
	if (IsValid(CurrentlyLoadedSaveGameObject))
	{
		UGameplayStatics::AsyncSaveGameToSlot(CurrentlyLoadedSaveGameObject, FString::Printf(TEXT("SaveGame%d"), LastSlotIndex), 0);
	}
}*/

void UMGPlayerDataSubsystem::LoadGameDelegateFunction(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
	if (UserIndex == 0)
	{
		FString Temp;
		SlotName.Split("SaveGame", nullptr, &Temp);

		CurrentlyLoadedSaveGameObject = Cast<UMGSaveGame>(LoadedGameData);
		CurrentlyLoadedSaveGameObject->SlotIndex = FCString::Atoi(*Temp);
	}
}
