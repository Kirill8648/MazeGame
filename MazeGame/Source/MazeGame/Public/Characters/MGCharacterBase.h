// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "MazeGame/MazeGame.h"
#include "MGCharacterBase.generated.h"

UCLASS()
class MAZEGAME_API AMGCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AMGCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase")
	virtual bool IsAlive() const;

	/*virtual void RemoveCharacterAbilities();

	virtual void Die();*/

	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase")
	virtual void FinishDying();

	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetHealthRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetEnergy() const;

	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetMaxEnergy() const;

	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetEnergyRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetStaminaRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetCoins() const;

protected:
	UPROPERTY()
	class UMGAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UMGAttributeSetBase* AttributeSetBase;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|MGCharacterBase")
	FText CharacterName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|Abilities")
	TArray<TSubclassOf<class UMGGameplayAbilityBase>> CharacterAbilities;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|Abilities")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	virtual void BeginPlay() override;

	virtual void AddCharacterAbilities();

	virtual void InitializeAttributes();

	virtual void AddStartupEffects();

	virtual void SetHealth(float Health);
};
