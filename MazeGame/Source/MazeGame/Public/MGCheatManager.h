// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "MGCheatManager.generated.h"

/**
 * Менеджер читов.
 */
UCLASS()
class MAZEGAME_API UMGCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	/**
	* Добавляет вылюту игроку. Работает только в лобби.
	* @param Amount - индекс слота сохранения
	*/
	UFUNCTION(Exec)
	void AddMoney(int Amount) const;
	/**
	* Разблокировывает все способности. Чтобы увидеть изменения нужно закрыть и открыть меню способностей.
	*/
	UFUNCTION(Exec)
	void UnlockAbilities() const;
};
