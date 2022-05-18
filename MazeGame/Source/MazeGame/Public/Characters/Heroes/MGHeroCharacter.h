// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/MGCharacterBase.h"
#include "MGHeroCharacter.generated.h"

/**
 * Класс управляемого игроком персонажа.
 */
UCLASS()
class MAZEGAME_API AMGHeroCharacter : public AMGCharacterBase
{
	GENERATED_BODY()

public:
	/**
	* Стандартный конструктор.
	*/
	AMGHeroCharacter();
	/**
	* Вызывается при репликации класса.
	* @param OutLifetimeProps - массив реплицируемых данных
	*/
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	/**
	* Инициализирует ввод с клавиатуры для персонажа.
	* @param PlayerInputComponent - указатель на компонент ввода для этого персонажа
	*/
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	/**
	* Вызывается когда персонаж начинает контролироваться контроллером.
	* @param NewController - указатель на новый контроллер
	*/
	virtual void PossessedBy(AController* NewController) override;
	/**
	* Обновляет способности в компоненте системы способностей.
	*/
	virtual void Die() override;

	UFUNCTION(BlueprintCallable, Category = "MazeGame|Player")
	void UpdateAbilities();
	/**
	* Функция, реализованная в наследуемом от этого класса блупринте. Удаляет слот удаленной способности в пользовательском интерфейсе.
	* @param SlotID - идентификатор ввода способности
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "MazeGame|HUD")
	void K2_RemoveAbilitySlotFromHUD(EMGAbilityInputID const& SlotID);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MazeGame|Camera")
	float BaseTurnRate; /**< Стандартная скорость поворота камеры персонажа. */

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MazeGame|Camera")
	float BaseLookUpRate; /**< Стандартная скорость поднятия камеры персонажа. */

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MazeGame|Flying")
	bool bMoveByCamera; /**< Двигаться ли параллельно камере в режиме полета. */

	bool bASCInputBound; /**< Привязан ли ввод компонента системы способностей к компоненту ввода персонажа. */

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "MazeGame|Camera")
	class UCameraComponent* FirstPersonCamera; /**< Указатель на компонент камеры персонажа. */

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|Camera")
	UPhysicalMaterial* DeadBoxPhysMat;

	UFUNCTION(BlueprintImplementableEvent, Category = "MazeGame|Player")
	void K2_OnDied();

	/**
	* Функция, вызывающаяся в момент начала игры.
	*/
	virtual void BeginPlay() override;
	/**
	* Функция, вызывающаяся в момент окончания игры.
	* @param EndPlayReason - причина окончания игры
	*/
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	/**
	* Функция, вызывающаяся после инииализации компонентов.
	*/
	virtual void PostInitializeComponents() override;
	/**
	* Посмотреть вверх.
	* @param Value - значение
	*/
	void LookUp(float Value);
	/**
	* Повернуть камеру.
	* @param Value - значение
	*/
	void Turn(float Value);
	/**
	* Двигаться вперед.
	* @param Value - значение
	*/
	void MoveForward(float Value);
	/**
	* Посмотреть вправо.
	* @param Value - значение
	*/
	void MoveRight(float Value);
	/**
	* Двигаться вверх.
	* @param Value - значение
	*/
	void MoveUp(float Value);
	/**
	* Привязывает ввод компонента системы способностей к компоненту ввода персонажа.
	*/
	void BindASCInput();
	/**
	* Вызывается при репликации состояния игрока.
	*/
	virtual void OnRep_PlayerState() override;
	/**
	* Функция, реализованная в наследуемом от этого класса блупринте. Начинает тряску камеры при ходьбе.
	* @param DoShakeOnStart - начинать ли тряску сразу или после задержки
	*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "MazeGame|Camera")
	void StartWalkCameraShake(int32 const& DoShakeOnStart);
	/**
	* Функция, реализованная в наследуемом от этого класса блупринте. Останавливает тряску камеры при ходьбе.
	* @param Immediately - закончить ли тряску моментально
	* @param ClearHandle - очистить ли таймер обнолвения тряски камеры
	*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "MazeGame|Camera")
	void StopWalkCameraShake(int32 const& Immediately, int32 const& ClearHandle);

private:
	bool bDoOnceMoveForwardStartShake;
	bool bDoOnceMoveForwardStopShake;

	bool bDoOnceMoveRightStartShake;
	bool bDoOnceMoveRightStopShake;
};
