// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "MGAttributeSetBase.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Набор атрибутов персонажа.
 */
UCLASS()
class MAZEGAME_API UMGAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:
	/**
	* Стандартный конструктор.
	*/
	UMGAttributeSetBase();

	UPROPERTY(BlueprintReadOnly, Category="Health", ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health; /**< Атрибут здоровья. */
	ATTRIBUTE_ACCESSORS(UMGAttributeSetBase, Health); /**< Макрос создания геттеров и сеттеров для атрибута здоровье. */

	UPROPERTY(BlueprintReadOnly, Category="Health", ReplicatedUsing=OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth; /**< Атрибут максимального количества здоровья. */
	ATTRIBUTE_ACCESSORS(UMGAttributeSetBase, MaxHealth); /**< Макрос создания геттеров и сеттеров для атрибута максимальное количество здоровья. */

	UPROPERTY(BlueprintReadOnly, Category="Health", ReplicatedUsing=OnRep_HealthRegenRate)
	FGameplayAttributeData HealthRegenRate; /**< Атрибут скорости восстановления здоровья. */
	ATTRIBUTE_ACCESSORS(UMGAttributeSetBase, HealthRegenRate); /**< Макрос создания геттеров и сеттеров для атрибута скорость восстановления здоровья. */

	UPROPERTY(BlueprintReadOnly, Category="Energy", ReplicatedUsing=OnRep_Energy)
	FGameplayAttributeData Energy; /**< Атрибут энергии. */
	ATTRIBUTE_ACCESSORS(UMGAttributeSetBase, Energy); /**< Макрос создания геттеров и сеттеров для атрибута энергия. */

	UPROPERTY(BlueprintReadOnly, Category="Energy", ReplicatedUsing=OnRep_MaxEnergy)
	FGameplayAttributeData MaxEnergy; /**< Атрибут максимального количества энергии. */
	ATTRIBUTE_ACCESSORS(UMGAttributeSetBase, MaxEnergy); /**< Макрос создания геттеров и сеттеров для атрибута максимальное количество энергии. */

	UPROPERTY(BlueprintReadOnly, Category="Energy", ReplicatedUsing=OnRep_EnergyRegenRate)
	FGameplayAttributeData EnergyRegenRate; /**< Атрибут скорости восстановления энергии. */
	ATTRIBUTE_ACCESSORS(UMGAttributeSetBase, EnergyRegenRate); /**< Макрос создания геттеров и сеттеров для атрибута скорость восстановления энергии. */

	UPROPERTY(BlueprintReadOnly, Category="Stamina", ReplicatedUsing=OnRep_Stamina)
	FGameplayAttributeData Stamina; /**< Атрибут стамины. */
	ATTRIBUTE_ACCESSORS(UMGAttributeSetBase, Stamina); /**< Макрос создания геттеров и сеттеров для атрибута стамина. */

	UPROPERTY(BlueprintReadOnly, Category="Stamina", ReplicatedUsing=OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina; /**< Атрибут максимального количества стамины. */
	ATTRIBUTE_ACCESSORS(UMGAttributeSetBase, MaxStamina); /**< Макрос создания геттеров и сеттеров для атрибута максимальное количество стамины. */

	UPROPERTY(BlueprintReadOnly, Category="Stamina", ReplicatedUsing=OnRep_StaminaRegenRate)
	FGameplayAttributeData StaminaRegenRate; /**< Атрибут скорости восстановления стамины. */
	ATTRIBUTE_ACCESSORS(UMGAttributeSetBase, StaminaRegenRate); /**< Макрос создания геттеров и сеттеров для атрибута скорость восстановления стамины. */

	UPROPERTY(BlueprintReadOnly, Category="Coins", ReplicatedUsing=OnRep_Coins)
	FGameplayAttributeData Coins; /**< Атрибут валюты. */
	ATTRIBUTE_ACCESSORS(UMGAttributeSetBase, Coins); /**< Макрос создания геттеров и сеттеров для атрибута валюта. */

	/**
	* Вызывается перед изменением атрибута.
	* @param Attribute - изменяемый атрибут
	* @param NewValue - новое значение атрибута
	*/
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	/**
	* Вызывается после применения эффекта игрового процесса.
	* @param Data - структура данных о эффекте игрового процесса
	*/
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	/**
	* Вызывается при репликации класса.
	* @param OutLifetimeProps - массив реплицируемых данных
	*/
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	/**
	* Вспомогательная функция для пропорционального изменения значения атрибута когда его максимально значение изменяется.
	* @param AffectedAttribute - изменяемый атрибут
	* @param MaxAttribute - максимальное значение атрибута
	* @param NewMaxValue - новое максимальное значение
	* @param AffectedAttributeProperty - затронутый атрибут
	*/
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue,
	                                 const FGameplayAttribute& AffectedAttributeProperty);

	/**
	* Вызывается при репликации здоровья.
	* @param OldHealth - предыдущее значение здоровья
	*/
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
	/**
	* Вызывается при репликации максимального здоровья.
	* @param OldMaxHealth - предыдущее значение максимального здоровья
	*/
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	/**
	* Вызывается при репликации скорости восстановаления здоровья.
	* @param OldHealthRegenRate - предыдущее значение скорости восстановаления здоровья
	*/
	UFUNCTION()
	virtual void OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate);
	/**
	* Вызывается при репликации энергии.
	* @param OldEnergy - предыдущее значение энергии
	*/
	UFUNCTION()
	virtual void OnRep_Energy(const FGameplayAttributeData& OldEnergy);
	/**
	* Вызывается при репликации максимальной энергии.
	* @param OldMaxEnergy - предыдущее максимальное значение энергии
	*/
	UFUNCTION()
	virtual void OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy);
	/**
	* Вызывается при репликации скорости восстановаления энергии.
	* @param OldEnergyRegenRate - предыдущее значение скорости восстановаления энергии
	*/
	UFUNCTION()
	virtual void OnRep_EnergyRegenRate(const FGameplayAttributeData& OldEnergyRegenRate);
	/**
	* Вызывается при репликации стамины.
	* @param OldStamina - предыдущее значение стамины
	*/
	UFUNCTION()
	virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);
	/**
	* Вызывается при репликации максимальной стамины.
	* @param OldMaxStamina - предыдущее максимальное значение стамины
	*/
	UFUNCTION()
	virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);
	/**
	* Вызывается при репликации скорости восстановаления стамины.
	* @param OldStaminaRegenRate - предыдущее значение скорости восстановаления стамины
	*/
	UFUNCTION()
	virtual void OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate);
	/**
	* Вызывается при репликации валюты.
	* @param OldCoins - предыдущее значение валюты
	*/
	UFUNCTION()
	virtual void OnRep_Coins(const FGameplayAttributeData& OldCoins);
};
