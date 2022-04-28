// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AbilitySystemComponent.h"
#include "MGAsyncTaskAttributeChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChanged, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);

/**
 * Блупринт нода, которая автоматически регистрирует listener для изменения значения атрибута в компоненте системы способностей.
 * Полезно использовать в пользовательском интерфейсе.
 */
UCLASS(BlueprintType, meta=(ExposedAsyncProxy = AsyncTask))
class MAZEGAME_API UMGAsyncTaskAttributeChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnAttributeChanged; /**< Делегат, вызывающийся при изменениизначения атрибута. */

	/**
	* Слушает изменение атрибута.
	* @param AbilitySystemComponent - компонент системы способностей
	* @param Attribute - атрибут
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UMGAsyncTaskAttributeChanged* ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute);
	/**
	* Слушает изменение атрибутов.
	* Версия с массивом атрибутов.
	* @param AbilitySystemComponent - компонент системы способностей
	* @param Attributes - массив атрибут
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UMGAsyncTaskAttributeChanged* ListenForAttributesChange(UAbilitySystemComponent* AbilitySystemComponent, TArray<FGameplayAttribute> Attributes);
	/**
	* Уничтожает задачу. Эту функцию нужно вызвать, когда задача больше не нужна, иначе задача будет существовать бесконечно.
	*/
	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:
	UPROPERTY()
	UAbilitySystemComponent* ASC; /**< Указатель на компонент системы способностей.*/

	FGameplayAttribute AttributeToListenFor; /**< Атрибут, чьи изменения нужно слушать.*/
	TArray<FGameplayAttribute> AttributesToListenFor; /**< Массив атрибутов, чьи изменения нужно слушать.*/
	/**
	* Вызывается при изменении атрибута.
	* @param Data - данные о изменении атрибута
	*/
	void AttributeChanged(const FOnAttributeChangeData& Data);
};
