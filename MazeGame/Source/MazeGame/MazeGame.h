// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MazeGame.generated.h"

UENUM(BlueprintType)
enum class EMGAbilityInputID : uint8
{
	None UMETA(DisplayName = "None"),
	Confirm UMETA(DisplayName = "Confirm"),
	Cancel UMETA(DisplayName = "Cancel"),
	ActivateFirst UMETA(DisplayName = "ActivateFirst"),
	ActivateSecond UMETA(DisplayName = "ActivateSecond"),
	ActivateThird UMETA(DisplayName = "ActivateThird"),
	ActivateFourth UMETA(DisplayName = "ActivateFourth")
};
