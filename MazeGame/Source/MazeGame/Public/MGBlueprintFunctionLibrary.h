// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MGBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class MAZEGAME_API UMGBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure)
	static FString SanitizeFloat(double InFloat, const int32 InMinFractionalDigits);
};