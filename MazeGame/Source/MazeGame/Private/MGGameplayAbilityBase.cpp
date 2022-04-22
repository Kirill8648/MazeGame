// Fill out your copyright notice in the Description page of Project Settings.


#include "MGGameplayAbilityBase.h"

UMGGameplayAbilityBase::UMGGameplayAbilityBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UMGGameplayAbilityBase::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	K2_OnAbilityAdded();
	Super::OnGiveAbility(ActorInfo, Spec);
}

void UMGGameplayAbilityBase::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	K2_OnAbilityRemoved();
	Super::OnRemoveAbility(ActorInfo, Spec);
}
