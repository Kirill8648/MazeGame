// Fill out your copyright notice in the Description page of Project Settings.


#include "MGCheatManager.h"

#include "MGPlayerDataSubsystem.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"

/*void UMGCheatManager::InitCheatManager()
{
	for (TFieldIterator<UFunction> Iterator(GetClass(), EFieldIteratorFlags::ExcludeSuper); Iterator; ++Iterator)
	{
		UFunction* Function = *Iterator;

		if (Function && !Function->HasAnyFunctionFlags(FUNC_Native))
		{
			Function->FunctionFlags = FUNC_Exec | FUNC_BlueprintCallable;
		}
	}
	
	Super::InitCheatManager();
}*/

void UMGCheatManager::UnlockAbilities() const
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	if (UGameplayStatics::GetCurrentLevelName(GetWorld()).Equals("Lobby"))
	{
		if (const UMGPlayerDataSubsystem* PlayerDataSubsystem = GameInstance->GetSubsystem<UMGPlayerDataSubsystem>())
			if (PlayerDataSubsystem->CurrentlyLoadedSaveGameObject)
			{
				for (FSavedAbilityInfo& AbilitiesLevel : PlayerDataSubsystem->CurrentlyLoadedSaveGameObject->AbilitiesLevels)
					if (AbilitiesLevel.AbilityInfo == 0)
						AbilitiesLevel.AbilityInfo = 1;
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, "Abilities unlocked, redraw UI to see changes");
			}
			else GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "No save file loaded");
		//else GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "Error during unlocking. PlayerDataSubsystem is null");
	}
	else GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "You're not in Lobby!");
}

void UMGCheatManager::AddMoney(const int Amount) const
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	const UMGPlayerDataSubsystem* PlayerDataSubsystem = GameInstance->GetSubsystem<UMGPlayerDataSubsystem>();

	//UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UMGPlayerDataSubsystem>()->CurrentlyLoadedSaveGameObject->PlayerCurrentCoinsNumber += Amount;
	if (UGameplayStatics::GetCurrentLevelName(GetWorld()).Equals("Lobby"))
	{
		if (PlayerDataSubsystem)
		{
			if (PlayerDataSubsystem->CurrentlyLoadedSaveGameObject)
			{
				PlayerDataSubsystem->CurrentlyLoadedSaveGameObject->PlayerCurrentCoinsNumber += Amount;
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, "Money Added");
			}
			else GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "No save file loaded");
		}
	}
	else GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "You're not in Lobby!");
}

void UMGCheatManager::RenderRealTimePortals(bool Render) const
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	if (const UMGPlayerDataSubsystem* PlayerDataSubsystem = GameInstance->GetSubsystem<UMGPlayerDataSubsystem>())
	{
		PlayerDataSubsystem->bRealTimePortals = Render;
	}
}

void UMGCheatManager::SetPortalsUpdateRate(float Rate) const
{
	const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	if (const UMGPlayerDataSubsystem* PlayerDataSubsystem = GameInstance->GetSubsystem<UMGPlayerDataSubsystem>())
	{
		PlayerDataSubsystem->PortalsUpdateRate = Rate;
	}
}

void UMGCheatManager::RemoveStremedLevel(ULevelStreaming* StreamingToRemove)
{
	ULevel* level = StreamingToRemove->GetLoadedLevel();
	if (level) StreamingToRemove->GetWorld()->RemoveFromWorld(level);
	/*else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Red, "No LoadedLevel!");
	}*/

	if (StreamingToRemove->GetCurrentState() == ULevelStreaming::ECurrentState::Loading)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Yellow, "Loading Overlap! Removing...");
		//StreamingToRemove->HasLoadRequestPending();
		/*StreamingToRemove->SetShouldBeLoaded(false);*/
		//StreamingToRemove->OnLevelLoaded.AddDynamic(this, UMGCheatManager::OnLevelLoaded());
		StreamingToRemove->SetIsRequestingUnloadAndRemoval(true);
		//StreamingToRemove->SetShouldBeVisible(false);
	}
	if (StreamingToRemove)
	{
		GetWorld()->RemoveStreamingLevel(StreamingToRemove);
		StreamingToRemove->MarkAsGarbage();
	}
	//StreamingToRemove->GetCurrentState()==ULevelStreaming::ECurrentState::Removed;
}

void UMGCheatManager::OnLevelLoaded()
{
}
