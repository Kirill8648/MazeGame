// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MGMazeGenerationProgressWidget.h"

void UMGMazeGenerationProgressWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//CanvasPanel->AddChildToCanvas(ScaleBox);
	ScaleBox->AddChild(TextBlock);

	/*if (ItemTitle)
	{
		ItemTitle->SetText(FText::FromString(TEXT("Hello world!")));
	}*/
}
