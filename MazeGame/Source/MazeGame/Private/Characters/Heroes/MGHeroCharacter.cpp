// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Heroes/MGHeroCharacter.h"

#include "MGAbilitySystemComponent.h"
#include "MGPlayerDataSubsystem.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AMGHeroCharacter::AMGHeroCharacter()
{
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;
	bASCInputBound = false;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(FName("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(RootComponent);
	FirstPersonCamera->bUsePawnControlRotation = true;
}

void AMGHeroCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AMGHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMGHeroCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMGHeroCharacter::MoveRight);

	PlayerInputComponent->BindAxis("MoveUp", this, &AMGHeroCharacter::MoveUp);

	PlayerInputComponent->BindAxis("LookUp", this, &AMGHeroCharacter::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &AMGHeroCharacter::Turn);

	BindASCInput();
}

void AMGHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAttributes();
	AddStartupEffects();
	//if (!AbilitySystemComponent->bCharacterAbilitiesGiven) UpdateAbilities();
	//UpdateAbilities();
	//AddCharacterAbilities();
}

void AMGHeroCharacter::Die()
{
	UBoxComponent* DeadBoxCollision = NewObject<UBoxComponent>(this);

	if (DeadBoxCollision)
	{
		DeadBoxCollision->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		DeadBoxCollision->RegisterComponent();
		DeadBoxCollision->InitBoxExtent(FVector(5.0f, 5.0f, 5.0f));
		DeadBoxCollision->SetGenerateOverlapEvents(false);
		DeadBoxCollision->SetCollisionProfileName("BlockAll");
		DeadBoxCollision->SetPhysMaterialOverride(DeadBoxPhysMat);
		DeadBoxCollision->SetRelativeTransform(FirstPersonCamera->GetRelativeTransform());
		DeadBoxCollision->SetPhysicsMaxAngularVelocityInDegrees(700.0f);
		FirstPersonCamera->AttachToComponent(DeadBoxCollision, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		FirstPersonCamera->bUsePawnControlRotation = false;
		FirstPersonCamera->bLockToHmd = false;
		DisableInput(Cast<APlayerController>(GetController()));
		DeadBoxCollision->SetSimulatePhysics(true);
		DeadBoxCollision->AddTorqueInDegrees(FVector(FMath::FRandRange(-7200.0f, 7200.0f), FMath::FRandRange(-7200.0f, 7200.0f), FMath::FRandRange(-7200.0f, 7200.0f)),
		                                     "NAME_None", true);
		DeadBoxCollision->AddImpulse(ClampVector(GetCharacterMovement()->GetLastUpdateVelocity(), FVector(-1850.0f, -1850.0f, -1850.0f), FVector(1850.0f, 1850.0f, 1850.0f)),
		                             "NAME_None", true);
	}
	K2_OnDied();
	Super::Die();
}

void AMGHeroCharacter::UpdateAbilities()
{
	//AbilitySystemComponent->ClearAllAbilities();

	TArray<FGameplayAbilitySpecHandle> Handles;
	AbilitySystemComponent->GetAllAbilities(Handles);
	for (FGameplayAbilitySpecHandle Handle : Handles)
	{
		//Cast<UMGGameplayAbilityBase>(AbilitySystemComponent->FindAbilitySpecFromHandle(Handle)->Ability)->K2_OnAbilityRemoved();
		K2_RemoveAbilitySlotFromHUD(Cast<UMGGameplayAbilityBase>(AbilitySystemComponent->FindAbilitySpecFromHandle(Handle)->Ability)->AbilityInputID);
		AbilitySystemComponent->ClearAbility(Handle);
	}

	AddCharacterAbilities();

	const UMGPlayerDataSubsystem* PlayerDataSubsystem = GetGameInstance()->GetSubsystem<UMGPlayerDataSubsystem>();

	if (PlayerDataSubsystem && PlayerDataSubsystem->CurrentlyLoadedSaveGameObject)
		for (FSavedAbilityInfo AbilitySlotNumber : PlayerDataSubsystem->CurrentlyLoadedSaveGameObject->AbilitiesSlotsNumbers)
		{
			if (!AbilitySlotNumber.AbilityKey.IsEmpty())
			{
				for (FSavedAbilityInfo AbilityLevel : PlayerDataSubsystem->CurrentlyLoadedSaveGameObject->AbilitiesLevels)
				{
					if (AbilityLevel.AbilityKey.Equals(AbilitySlotNumber.AbilityKey))
					{
						EMGAbilityInputID AbilityInputID;
						switch (AbilitySlotNumber.AbilityInfo)
						{
						case 0:
							AbilityInputID = EMGAbilityInputID::ActivateFirst;
							break;
						case 1:
							AbilityInputID = EMGAbilityInputID::ActivateSecond;
							break;
						case 2:
							AbilityInputID = EMGAbilityInputID::ActivateThird;
							break;
						case 3:
							AbilityInputID = EMGAbilityInputID::ActivateFourth;
							break;
						case 4:
							AbilityInputID = EMGAbilityInputID::ActivateFifth;
							break;
						default:
							AbilityInputID = EMGAbilityInputID::None;
							GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "ERROR: Given ability will not have an input ID");
						}

						FGameplayAbilitySpec NewSpec = FGameplayAbilitySpec(
							PlayerDataSubsystem->AbilitiesTable->FindRow<FAbilityLevelPrices>(FName(*AbilityLevel.AbilityKey), FString())->Ability, AbilityLevel.AbilityInfo,
							static_cast<int32>(AbilityInputID), this);

						if (NewSpec.Ability)
						{
							CastChecked<UMGGameplayAbilityBase, UGameplayAbility>(NewSpec.Ability)->AbilityInputID = AbilityInputID;
							CastChecked<UMGGameplayAbilityBase, UGameplayAbility>(NewSpec.Ability)->AbilityKey = AbilityLevel.AbilityKey;
							/*CastChecked<UMGGameplayAbilityBase, UGameplayAbility>(NewSpec.Ability)->MaxLvl = PlayerDataSubsystem->AbilitiesTable->FindRow<FAbilityLevelPrices>(
								*AbilityLevel.AbilityKey, "")->MaxLvl;*/
							/*Cast<UMGGameplayAbilityBase>(NewSpec.Ability)->AbilityInputID = AbilityInputID;
							Cast<UMGGameplayAbilityBase>(NewSpec.Ability)->AbilityKey = AbilityLevel.AbilityKey;*/
							AbilitySystemComponent->GiveAbility(NewSpec);
						}
						break;
					}
				}
			}
		}
	AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}

void AMGHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
	//UpdateAbilities();
}

void AMGHeroCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AMGHeroCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMGHeroCharacter::LookUp(float Value)
{
	if (IsAlive()) AddControllerPitchInput(Value);
}

void AMGHeroCharacter::Turn(float Value)
{
	if (IsAlive()) AddControllerYawInput(Value);
}

void AMGHeroCharacter::MoveForward(float Value)
{
	if (IsAlive())
	{
		if (bMoveByCamera && GetMovementComponent()->IsFlying())
		{
			AddMovementInput(FirstPersonCamera->GetForwardVector(), Value);
		}
		else
		{
			AddMovementInput(UKismetMathLibrary::GetForwardVector(FRotator(0, GetControlRotation().Yaw, 0)), Value);
			if (!GetMovementComponent()->IsFlying())
			{
				if (!GetMovementComponent()->IsFalling() && (GetVelocity().X > 1.0f || GetVelocity().X < -1.0f || GetVelocity().Y > 1.0f || GetVelocity().Y < -1.0f))
				{
					if (!bDoOnceMoveForwardStartShake)
					{
						bDoOnceMoveForwardStartShake = true;
						bDoOnceMoveForwardStopShake = false;

						/*GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString::Printf(TEXT("Playing walk shake")));
						UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StopAllCameraShakes(false);
						UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(WalkCameraShake.Get());*/
						StartWalkCameraShake(1);
					}
				}
				else
				{
					if (!bDoOnceMoveForwardStopShake)
					{
						bDoOnceMoveForwardStartShake = false;
						bDoOnceMoveForwardStopShake = true;

						/*GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString::Printf(TEXT("Stoping walk shake")));
						UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StopAllCameraShakes(false);*/
						StopWalkCameraShake(1, 1);
					}
				}
			}
		}
	}
}

void AMGHeroCharacter::MoveRight(float Value)
{
	if (IsAlive())
	{
		if (bMoveByCamera && GetMovementComponent()->IsFlying())
		{
			AddMovementInput(FirstPersonCamera->GetRightVector(), Value);
		}
		else
		{
			AddMovementInput(UKismetMathLibrary::GetRightVector(FRotator(0, GetControlRotation().Yaw, 0)), Value);
		}
	}
}

void AMGHeroCharacter::MoveUp(float Value)
{
	if (IsAlive() && GetMovementComponent()->IsFlying())
	{
		AddMovementInput(UKismetMathLibrary::GetUpVector(FRotator(0, GetControlRotation().Yaw, 0)), Value);
	}
}

void AMGHeroCharacter::BindASCInput()
{
	if (!bASCInputBound && IsValid(AbilitySystemComponent) && IsValid(InputComponent))
	{
		const FGameplayAbilityInputBinds Binds("Confirm", "Cancel", "EMGAbilityInputID",
		                                       static_cast<int32>(EMGAbilityInputID::Confirm),
		                                       static_cast<int32>(EMGAbilityInputID::Cancel));
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);

		bASCInputBound = true;
	}
}

void AMGHeroCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAttributes();
	BindASCInput();
}
