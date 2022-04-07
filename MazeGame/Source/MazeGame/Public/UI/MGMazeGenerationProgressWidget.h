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
 * 
 */
UCLASS()
class MAZEGAME_API UMGMazeGenerationProgressWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	/*UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CanvasPanel;*/

	UPROPERTY(meta = (BindWidget))
	UScaleBox* ScaleBox;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock;
};
