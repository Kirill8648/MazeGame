// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MazeGame.generated.h"

/** 
 * Перечисление идентификаторов ввода для способностей.
 */
UENUM(BlueprintType)
enum class EMGAbilityInputID : uint8
{
	None UMETA(DisplayName = "None"),
	/**< Пустой идентификатор. */
	Confirm UMETA(DisplayName = "Confirm"),
	/**< Идентификатор "Подтвердить". */
	Cancel UMETA(DisplayName = "Cancel"),
	/**< Идентификатор "Отменить". */
	Sprint UMETA(DisplayName = "Sprint"),
	/**< Идентификатор "Бег". */
	ActivateFirst UMETA(DisplayName = "ActivateFirst"),
	/**< Идентификатор "Активировать первую способность". */
	ActivateSecond UMETA(DisplayName = "ActivateSecond"),
	/**< Идентификатор "Активировать вторую способность". */
	ActivateThird UMETA(DisplayName = "ActivateThird"),
	/**< Идентификатор "Активировать третью способность". */
	ActivateFourth UMETA(DisplayName = "ActivateFourth"),
	/**< Идентификатор "Активировать четвертую способность". */
	ActivateFifth UMETA(DisplayName = "ActivateFifth") /**< Идентификатор "Активировать пятую способность". */
};
