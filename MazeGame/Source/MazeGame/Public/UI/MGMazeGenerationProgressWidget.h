// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/ScaleBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "MGMazeGenerationProgressWidget.generated.h"

/**
 * Виджет загрузки генерации лабиринта.
 */
UCLASS()
class MAZEGAME_API UMGMazeGenerationProgressWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	* Вызывается в момент конструирования виджета.
	*/
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UScaleBox* ScaleBox;/**< Масштабируемая область. */

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock;/**< Блок текста. */
};
