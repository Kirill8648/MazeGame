// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MGBlueprintFunctionLibrary.generated.h"

/**
 * Библиотека функций для блупринтов
 */
UCLASS()
class MAZEGAME_API UMGBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/**
	* Обрезает дробное значение до указанного количества знаков после запятой.
	* @param InFloat - значение
	* @param InMinFractionalDigits - количества знаков после запятой
	* @return строка с обрезанным дробным значением
	*/
	UFUNCTION(BlueprintPure)
	static FString SanitizeFloat(double InFloat, const int32 InMinFractionalDigits);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void SpawnActorsInGrid(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, int32 X, int32 Y, float GridSize);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void DestroyAllActorsByClass(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass);

	UFUNCTION(BlueprintCallable)
	static UTexture2D* GetTextureFromRenderTarget2D(UTextureRenderTarget2D* TextureRenderTarget);

	UFUNCTION(BlueprintCallable)
	static UTextureCube* GetTextureFromRenderTargetCube(UTextureRenderTargetCube* TextureRenderTargetCube);
};
