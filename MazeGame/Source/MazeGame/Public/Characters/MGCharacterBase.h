// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "MazeGame/MazeGame.h"
#include "MGCharacterBase.generated.h"

/**
 * Базовый класс персонажа.
 * Любой персонаж, взаимодействующий со системой способностей должен наследовать от него.
 */
UCLASS()
class MAZEGAME_API AMGCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	/**
	* Стандартный конструктор.
	*/
	AMGCharacterBase();
	/**
	* Функция, возвращающая компонент системы способностей.
	* @return - компонент ситемы способностей
	*/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/**
	* Проверяет, жив ли персонаж.
	* @return - живой ли персонаж
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase")
	virtual bool IsAlive() const;

	virtual void RemoveCharacterAbilities();
	/**
	* Если нужно уничтожить актор после смерти, необходимо переопределить эту функцию и вызвать в ней FinishDying()
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase")
	virtual void Die();
	/**
	* Закончить умирать.
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase")
	virtual void FinishDying();
	/**
	* Геттер здоровья.
	* @return - здоровье
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetHealth() const;
	/**
	* Геттер максимального здоровья.
	* @return - максимальное здоровье
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetMaxHealth() const;
	/**
	* Геттер скорости восстановления здоровья.
	* @return - скорость восстановления здоровья
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetHealthRegenRate() const;
	/**
	* Геттер энергии.
	* @return - энергия
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetEnergy() const;
	/**
	* Геттер максимальной энергии.
	* @return - максимальная энергия
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetMaxEnergy() const;
	/**
	* Геттер скорости восстановления энергии.
	* @return - скорость восстановления энергии
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetEnergyRegenRate() const;
	/**
	* Геттер стамины.
	* @return - стамина
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetStamina() const;
	/**
	* Геттер максимальной стамины.
	* @return - максимальная стамина
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetMaxStamina() const;
	/**
	* Геттер скорости восстановления стамины.
	* @return - скорость восстановления стамины
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetStaminaRegenRate() const;
	/**
	* Геттер валюты.
	* @return - валюта
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGCharacterBase|Attributes")
	float GetCoins() const;

protected:
	UPROPERTY()
	class UMGAbilitySystemComponent* AbilitySystemComponent; /**< Указатель на компонент системы способностей. */

	UPROPERTY()
	class UMGAttributeSetBase* AttributeSetBase; /**< Указатель на набор атрибутов системы способностей. */

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|MGCharacterBase")
	FText CharacterName; /**< Имя персонажа. */

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|Abilities")
	TArray<TSubclassOf<class UMGGameplayAbilityBase>> CharacterAbilities; /**< Массив классов способностей, стандартных для этого персонажа. */

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes; /**< Массив классов эффектов игрового процесса, инициализирующих стандартные атрибуты для этого персонажа. */

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|Abilities")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects; /**< Массив классов эффектов игрового процесса, добавляющих стартовые эффекты для этого персонажа. */
	/**
	* Функция, вызывающаяся в момент начала игры.
	*/
	virtual void BeginPlay() override;
	/**
	* Добавляет каждую способность из CharacterAbilities к компоненту системы способностей.
	*/
	virtual void AddCharacterAbilities();
	/**
	* Инициализирует стандартные атрибуты для данного персонажа.
	*/
	virtual void InitializeAttributes();
	/**
	* Применяет каждый эффект из StartupEffects на компоненту системы способностей.
	*/
	virtual void AddStartupEffects();
	/**
	* Сеттер здоровья.
	* @param Health - здоровье 
	*/
	virtual void SetHealth(float Health);
};
