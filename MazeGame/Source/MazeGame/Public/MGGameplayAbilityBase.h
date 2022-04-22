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
	FString AbilityKey;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability")
	UDataTable* StatsTable;

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "OnAbilityAdded")
	void K2_OnAbilityAdded();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "OnAbilityRemoved")
	void K2_OnAbilityRemoved();
	
protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
};
