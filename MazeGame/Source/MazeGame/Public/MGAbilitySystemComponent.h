// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MGAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class MAZEGAME_API UMGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	UMGAbilitySystemComponent();

	bool bCharacterAbilitiesGiven = false;
	bool bStartupEffectsApplied = false;
};
