// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Abilities/MGMMC_HeroAbilityCost.h"
#include "MGGameplayAbilityBase.h"
#include "MGSaveGame.h"

float UMGMMC_HeroAbilityCost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const UMGGameplayAbilityBase* Ability = Cast<UMGGameplayAbilityBase>(Spec.GetContext().GetAbilityInstance_NotReplicated());

	if (!Ability)
	{
		return 0.0f;
	}
	if (const FAbilityStatsByLevels* CostStruct = Ability->StatsTable->FindRow<FAbilityStatsByLevels>(TEXT("Cost"), ""))
	{
		switch (Ability->GetAbilityLevel())
		{
		case 1: return CostStruct->Lvl1Value;
		case 2: return CostStruct->Lvl2Value;
		case 3: return CostStruct->Lvl3Value;
		case 4: return CostStruct->Lvl4Value;
		case 5: return CostStruct->Lvl5Value;
		case 6: return CostStruct->Lvl6Value;
		case 7: return CostStruct->Lvl7Value;
		case 8: return CostStruct->Lvl8Value;
		case 9: return CostStruct->Lvl9Value;
		case 10: return CostStruct->Lvl10Value;
		default: return 0.0f;
		}
	}
	return 0.0f;
}
