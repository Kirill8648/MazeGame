// Fill out your copyright notice in the Description page of Project Settings.


#include "MGBlueprintFunctionLibrary.h"

FString UMGBlueprintFunctionLibrary::SanitizeFloat(double InFloat, const int32 InMinFractionalDigits)
{
	return FString::SanitizeFloat(InFloat, InMinFractionalDigits);
}
