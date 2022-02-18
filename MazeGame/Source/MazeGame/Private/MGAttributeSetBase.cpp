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
