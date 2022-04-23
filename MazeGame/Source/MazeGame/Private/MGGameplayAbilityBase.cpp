// Fill out your copyright notice in the Description page of Project Settings.


#include "MGGameplayAbilityBase.h"

UMGGameplayAbilityBase::UMGGameplayAbilityBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
}

void UMGGameplayAbilityBase::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	K2_OnAbilityAdded();
}

void UMGGameplayAbilityBase::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
	K2_OnAbilityRemoved();
}

bool UMGGameplayAbilityBase::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bReturning = Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
	K2_OnAbilityCheckCost(bReturning);
	return bReturning;
}

bool UMGGameplayAbilityBase::CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                           FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bReturning = Super::CheckCooldown(Handle, ActorInfo, OptionalRelevantTags);
	K2_OnAbilityCheckCooldown(bReturning);
	return bReturning;
}
