// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MGAbilitySystemComponent.generated.h"

/**
 * Компонент системы способностей для этой игры.
 * Все классы, взаимодействующие со способностями должны использовать его.
 */
UCLASS()
class MAZEGAME_API UMGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	/**
	* Стандартный конструктор.
	*/
	UMGAbilitySystemComponent();

	bool bCharacterAbilitiesGiven = false; /**< Выданы ли способности игрока. */
	bool bStartupEffectsApplied = false; /**< Применены ли стартовые эффекты. */
};
