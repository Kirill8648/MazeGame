// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MazeGame/MazeGame.h"
#include "MGGameplayAbilityBase.generated.h"

/**
 * 
 */
UCLASS()
class MAZEGAME_API UMGGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UMGGameplayAbilityBase();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability")
	EMGAbilityInputID AbilityInputID = EMGAbilityInputID::None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability")
	EMGAbilityInputID AbilityID = EMGAbilityInputID::None;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability")
	UDataTable* StatsTable;
};
