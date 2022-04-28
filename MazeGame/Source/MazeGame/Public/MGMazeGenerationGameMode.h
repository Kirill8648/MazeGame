// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MGMazeGenerationGameMode.generated.h"

DECLARE_DELEGATE_TwoParams(FString2Delegate, FString, FString)
DECLARE_DELEGATE(FDelegate)

/**
 * Класс игрового режима, размещенный на уровне лабиринта и использующийся для генерации лабиринта.
 */
UCLASS()
class MAZEGAME_API AMGMazeGenerationGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	/**
	* Стандартный конструктор.
	*/
	AMGMazeGenerationGameMode();

	FString2Delegate ChangeLoadingScreenTextDelegate; /**< Делегат, вызывающийся когда нужно изменить текст загрузки. */

	FDelegate AllFinishedDelegate; /**< Делегат, вызывающийся когда генерация лабиринта завершена. */

	UPROPERTY(EditAnywhere, Category = "MGMazeGenerationGameMode|Widget")
	TSubclassOf<UUserWidget> LoadingScreenWidget; /**< Класс виджета загрузки. */

	UPROPERTY(BlueprintReadOnly, Category = "MGMazeGenerationGameMode|Widget")
	UUserWidget* LoadingScreenWidgetRef; /**< Указатель на объект виджета загрузки. */

	/**
	* Функция, реализованная в наследуемом от этого класса блупринте. Обновляет текст на виджете загрузки.
	* @param TableNamespace - имя таблица текстов
	* @param KeyInTable - ключ в таблице текстов
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void K2_UpdateLoadingScreenWidgetText(FString const& TableNamespace, FString const& KeyInTable);

protected:
	/**
	* Функция проверки возможномти начать игру.
	* @return можно ли начать игру
	*/
	virtual bool ReadyToStartMatch_Implementation() override;
	/**
	* Функция, вызывающаяся в момент начала игры.
	*/
	virtual void BeginPlay() override;
	/**
	* Функция, вызывающаяся в момент смены состояния игры.
	*/
	virtual void OnMatchStateSet() override;

private:
	bool bIsReadyToStart = false; /**< Можно ли начать игру. */
	/**
	* Функция, привязанная к делегату AllFinishedDelegate.
	*/
	void AllFinished();
	/**
	* Функция, привязанная к делегату ChangeLoadingScreenTextDelegate.
	*/
	void UpdateLoadingScreenWidgetTextHelperFunction(FString TableNamespace, FString KeyInTable);
};
