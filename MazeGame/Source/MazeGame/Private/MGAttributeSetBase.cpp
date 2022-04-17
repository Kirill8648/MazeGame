// Fill out your copyright notice in the Description page of Project Settings.


#include "MGAttributeSetBase.h"
#include "Net/UnrealNetwork.h"

UMGAttributeSetBase::UMGAttributeSetBase()
{
}

void UMGAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UMGAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMGAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMGAttributeSetBase, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMGAttributeSetBase, Energy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMGAttributeSetBase, MaxEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMGAttributeSetBase, EnergyRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMGAttributeSetBase, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMGAttributeSetBase, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMGAttributeSetBase, StaminaRegenRate, COND_None, REPNOTIFY_Always);
}

void UMGAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMGAttributeSetBase, Health, OldHealth);
}

void UMGAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMGAttributeSetBase, MaxHealth, OldMaxHealth);
}

void UMGAttributeSetBase::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMGAttributeSetBase, HealthRegenRate, OldHealthRegenRate);
}

void UMGAttributeSetBase::OnRep_Energy(const FGameplayAttributeData& OldEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMGAttributeSetBase, Energy, OldEnergy);
}

void UMGAttributeSetBase::OnRep_MaxEnergy(const FGameplayAttributeData& OldMaxEnergy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMGAttributeSetBase, MaxEnergy, OldMaxEnergy);
}

void UMGAttributeSetBase::OnRep_EnergyRegenRate(const FGameplayAttributeData& OldEnergyRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMGAttributeSetBase, EnergyRegenRate, OldEnergyRegenRate);
}

void UMGAttributeSetBase::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMGAttributeSetBase, Stamina, OldStamina);
}

void UMGAttributeSetBase::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMGAttributeSetBase, MaxStamina, OldMaxStamina);
}

void UMGAttributeSetBase::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMGAttributeSetBase, StaminaRegenRate, OldStaminaRegenRate);
}
