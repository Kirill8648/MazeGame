// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Abilities/MGMMC_HeroAbilityCooldown.h"
#include "MGGameplayAbilityBase.h"
#include "MGSaveGame.h"

float UMGMMC_HeroAbilityCooldown::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const UMGGameplayAbilityBase* Ability = Cast<UMGGameplayAbilityBase>(Spec.GetContext().GetAbilityInstance_NotReplicated());

	if (!Ability)
	{
		return 0.0f;
	}
	if (const FAbilityStatsByLevels* CooldownStruct = Ability->StatsTable->FindRow<FAbilityStatsByLevels>(TEXT("Cooldown"), ""))
	{
		switch (Ability->GetAbilityLevel())
		{
		case 1: return CooldownStruct->Lvl1Value;
		case 2: return CooldownStruct->Lvl2Value;
		case 3: return CooldownStruct->Lvl3Value;
		case 4: return CooldownStruct->Lvl4Value;
		case 5: return CooldownStruct->Lvl5Value;
		case 6: return CooldownStruct->Lvl6Value;
		case 7: return CooldownStruct->Lvl7Value;
		case 8: return CooldownStruct->Lvl8Value;
		case 9: return CooldownStruct->Lvl9Value;
		case 10: return CooldownStruct->Lvl10Value;
		default: return 0.0f;
		}
	}
	return 0.0f;
}
