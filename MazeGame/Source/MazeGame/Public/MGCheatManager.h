// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "MGCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class MAZEGAME_API UMGCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
public:
	//virtual void InitCheatManager() override;
	
	UFUNCTION(Exec)
	void AddMoney(int Amount) const;

	UFUNCTION(Exec)
	void UnlockAbilities() const;
};
