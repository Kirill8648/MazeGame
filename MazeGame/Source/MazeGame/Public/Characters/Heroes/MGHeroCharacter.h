// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/MGCharacterBase.h"
#include "MGHeroCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MAZEGAME_API AMGHeroCharacter : public AMGCharacterBase
{
	GENERATED_BODY()

public:
	AMGHeroCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintCallable, Category = "MazeGame|Player")
	void UpdateAbilities();

	UFUNCTION(BlueprintImplementableEvent, Category = "MazeGame|HUD")
	void K2_RemoveAbilitySlotFromHUD(EMGAbilityInputID const& SlotID);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MazeGame|Camera")
	float BaseTurnRate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MazeGame|Camera")
	float BaseLookUpRate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MazeGame|Flying")
	bool bMoveByCamera;

	bool bASCInputBound;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "MazeGame|Camera")
	class UCameraComponent* FirstPersonCamera;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PostInitializeComponents() override;

	void LookUp(float Value);

	void Turn(float Value);

	void MoveForward(float Value);

	void MoveRight(float Value);

	void MoveUp(float Value);

	void BindASCInput();

	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "MazeGame|Camera")
	void StartWalkCameraShake(int32 const &DoShakeOnStart);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "MazeGame|Camera")
	void StopWalkCameraShake(int32 const &Immediately, int32 const &ClearHandle);
	
private:
	bool bDoOnceMoveForwardStartShake;
	bool bDoOnceMoveForwardStopShake;

	bool bDoOnceMoveRightStartShake;
	bool bDoOnceMoveRightStopShake;
};
