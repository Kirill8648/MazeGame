// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MGMMC_HeroAbilityCost.generated.h"

/**
 * Класс расчета значения модификатора цены способности
 */
UCLASS()
class MAZEGAME_API UMGMMC_HeroAbilityCost : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	/**
	* Имплементация рассчета значения.
	* @param Spec - спецификация эффекта игрового процесса
	* @return - расчитанное значение модификатора
	*/
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
