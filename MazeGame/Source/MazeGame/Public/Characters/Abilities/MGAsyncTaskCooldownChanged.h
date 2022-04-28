// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "MGAsyncTaskCooldownChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCooldownChanged, FGameplayTag, CooldownTag, float, TimeRemaining, float, Duration);

/**
 * Блупринт нода, которая автоматически регистрирует listener для изменения (начала и конца) для массива тэгов перезарядки.
 * Полезно использовать в пользовательском интерфейсе.
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class MAZEGAME_API UMGAsyncTaskCooldownChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCooldownChanged OnCooldownBegin; /**< Делегат, вызывающийся при начале перезарядки. */

	UPROPERTY(BlueprintAssignable)
	FOnCooldownChanged OnCooldownEnd; /**< Делегат, вызывающийся по окончании перезарядки. */

	/**
	* Слушает измененения (начало и конец) дял эффекта игрового процесса перезарядки, основываясь на тэге перезарядки.
	* @param AbilitySystemComponent - компонент системы способностей
	* @param CooldownTags - контейнер тэгов перезарядки
	* @param UseServerCooldown - вызывать ли перезарядку дополнительно на сервере
	* @return - указатель на асинхронную задачу
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UMGAsyncTaskCooldownChanged* ListenForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayTagContainer CooldownTags, bool UseServerCooldown);
	/**
	* Уничтожает задачу. Эту функцию нужно вызвать, когда задача больше не нужна, иначе задача будет существовать бесконечно.
	*/
	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:
	UPROPERTY()
	UAbilitySystemComponent* ASC; /**< Указатель на компонент системы способностей.*/

	FGameplayTagContainer CooldownTags; /**< Контейнер тэгов перезарядки.*/

	bool UseServerCooldown; /**< Вызывать ли перезарядку дополнительно на сервере.*/
	/**
	* Вызывается когда добавленный активный эффект игрового процесса дает обратную связь.
	* @param Target - целевой компонент системы способностей
	* @param SpecApplied - спецификация эффекта игрового процесса
	* @param ActiveHandle - содержит информацию об активном эффекте игрового процесса
	*/
	virtual void OnActiveGameplayEffectAddedCallback(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);
	/**
	* Вызывается когда изменяется контейнер тэгов перезарядки.
	* @param CooldownTag - измененный тэг
	* @param NewCount - новое количество тэгов в контейнере
	*/
	virtual void CooldownTagChanged(const FGameplayTag CooldownTag, int32 NewCount);
	/**
	* Геттер состояния перезарядки для определенных тэгов.
	* @param CooldownTags - контейнер тэгов
	* @param TimeRemaining - оставшееся время перезарядки
	* @param CooldownDuration - время перезарядки
	* @return - закончилась ли перезарядка
	*/
	bool GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration);
};
