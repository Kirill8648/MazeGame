// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MGCharacterBase.h"
#include "MGAbilitySystemComponent.h"
#include "MGAttributeSetBase.h"
#include "MGGameplayAbilityBase.h"
#include "Components/CapsuleComponent.h"

AMGCharacterBase::AMGCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UMGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSetBase = CreateDefaultSubobject<UMGAttributeSetBase>(TEXT("AttributeSetBase"));
}

UAbilitySystemComponent* AMGCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool AMGCharacterBase::IsAlive() const
{
	return GetHealth() > 0.0f;
}

void AMGCharacterBase::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || !AbilitySystemComponent->bCharacterAbilitiesGiven)
	{
		return;
	}

	AbilitySystemComponent->ClearAllAbilities();
	AbilitySystemComponent->bCharacterAbilitiesGiven = false;
}

void AMGCharacterBase::Die()
{
	RemoveCharacterAbilities();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
		AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag("State.Dead"));
	}
}

void AMGCharacterBase::FinishDying()
{
	Destroy();
}

float AMGCharacterBase::GetHealth() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetHealth();
	}
	return 0.0f;
}

float AMGCharacterBase::GetMaxHealth() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetMaxHealth();
	}
	return 0.0f;
}

float AMGCharacterBase::GetHealthRegenRate() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetHealthRegenRate();
	}
	return 0.0f;
}

float AMGCharacterBase::GetEnergy() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetEnergy();
	}
	return 0.0f;
}

float AMGCharacterBase::GetMaxEnergy() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetMaxEnergy();
	}
	return 0.0f;
}

float AMGCharacterBase::GetEnergyRegenRate() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetEnergyRegenRate();
	}
	return 0.0f;
}

float AMGCharacterBase::GetStamina() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetStamina();
	}
	return 0.0f;
}

float AMGCharacterBase::GetMaxStamina() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetMaxStamina();
	}
	return 0.0f;
}

float AMGCharacterBase::GetStaminaRegenRate() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetStaminaRegenRate();
	}
	return 0.0f;
}

float AMGCharacterBase::GetCoins() const
{
	if (IsValid(AttributeSetBase))
	{
		return AttributeSetBase->GetCoins();
	}
	return 0.0f;
}

void AMGCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AMGCharacterBase::AddCharacterAbilities()
{
	for (TSubclassOf<UMGGameplayAbilityBase>& StartupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, 1, static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}

void AMGCharacterBase::InitializeAttributes()
{
	if (!IsValid(AbilitySystemComponent))
	{
		return;
	}

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."),
		       *FString(__FUNCTION__), *GetName())
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	//TODO apply levels correctly
	FGameplayEffectSpecHandle GEHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, 1, EffectContext);
	if (GEHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*GEHandle.Data.Get());
	}
}

void AMGCharacterBase::AddStartupEffects()
{
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	//TODO apply levels correctly
	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle GEHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
		if (GEHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(
				*GEHandle.Data.Get(), AbilitySystemComponent);
		}
	}
}

void AMGCharacterBase::SetHealth(float Health)
{
	if (IsValid(AttributeSetBase))
	{
		AttributeSetBase->SetHealth(Health);
	}
}
