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

	/*UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability")
	EMGAbilityInputID AbilityID = EMGAbilityInputID::None;*/

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability")
	FString AbilityKey;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability")
	FGameplayTagContainer CooldownTags;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability")
	UDataTable* StatsTable;

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "OnAbilityAdded")
	void K2_OnAbilityAdded();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "OnAbilityRemoved")
	void K2_OnAbilityRemoved();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "OnAbilityCheckCost")
	void K2_OnAbilityCheckCost(bool ReturnValue) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "OnAbilityCheckCooldown")
	void K2_OnAbilityCheckCooldown(bool ReturnValue) const;

protected:
	// Temp container that we will return the pointer to in GetCooldownTags().
	// This will be a union of our CooldownTags and the Cooldown GE's cooldown tags.
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual bool CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo) const override;
};
