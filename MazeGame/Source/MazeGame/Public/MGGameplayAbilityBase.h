// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MazeGame/MazeGame.h"
#include "MGGameplayAbilityBase.generated.h"

/**
 * Класс способности. От него должны наследоваться все способности в нашей игре.
 */
UCLASS()
class MAZEGAME_API UMGGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	/**
	* Стандартный конструктор.
	*/
	UMGGameplayAbilityBase();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability")
	EMGAbilityInputID AbilityInputID = EMGAbilityInputID::None; /**< Идентификатор ввода для данной способности. */

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability")
	FString AbilityKey; /**< Ключ данной способности. */

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability")
	FGameplayTagContainer CooldownTags; /**< Тэги перезарядки для данной способности. */

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Ability")
	UDataTable* StatsTable; /**< Таблица характеристик данной способности. */

	/**
	* Функция, реализованная в наследуемом от этого класса блупринте. Вызывается в момент добавления способности.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "OnAbilityAdded")
	void K2_OnAbilityAdded();

	/**
	* Функция, реализованная в наследуемом от этого класса блупринте. Вызывается в момент удаления способности.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "OnAbilityRemoved")
	void K2_OnAbilityRemoved();

	/**
	* Функция, реализованная в наследуемом от этого класса блупринте. Вызывается в момент проверки цены способности.
	* @param ReturnValue - прошла ли способность проверку
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "OnAbilityCheckCost")
	void K2_OnAbilityCheckCost(bool ReturnValue) const;

	/**
	* Функция, реализованная в наследуемом от этого класса блупринте. Вызывается в момент проверки перезарядки способности.
	* @param ReturnValue - прошла ли способность проверку
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "OnAbilityCheckCooldown")
	void K2_OnAbilityCheckCooldown(bool ReturnValue) const;

protected:
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;
	/**< Временный контейнер, указатель на который мы вернем в GetCooldownTags(). Это будет объединение состоящие из CooldownTags и Cooldown GE's cooldown tags*/

	/**
	* Вызывается в момент выдачи способности.
	* @param ActorInfo - информация об акторе, владеющем этой способностью
	* @param Spec - структура спецификации способности
	*/
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	/**
	* Вызывается в момент удаления способности.
	* @param ActorInfo - информация об акторе, владеющем этой способностью
	* @param Spec - структура спецификации способности
	*/
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	/**
	* Вызывается в момент проверки цены способности.
	* @param Handle - обработчик структуры спецификации способности
	* @param ActorInfo - информация об акторе, владеющем этой способностью
	* @param OptionalRelevantTags - необязательные дополнительные тэги
	* @return прошла ли способность проверку.
	*/
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	/**
	* Вызывается в момент преверки перезарядки способности.
	* @param Handle - обработчик структуры спецификации способности
	* @param ActorInfo - информация об акторе, владеющем этой способностью
	* @param OptionalRelevantTags - необязательные дополнительные тэги
	* @return прошла ли способность проверку.
	*/
	virtual bool CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           FGameplayTagContainer* OptionalRelevantTags) const override;
	/**
	* Вызывается внутри CheckCooldown().
	* @return указатель на TempCooldownTags
	*/
	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	/**
	* Вызывается в момент применения перезарядки способности.
	* @param Handle - обработчик структуры спецификации способности
	* @param ActorInfo - информация об акторе, владеющем этой способностью
	* @param ActivationInfo - структура с информацией об активации способности
	*/
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo) const override;
};
