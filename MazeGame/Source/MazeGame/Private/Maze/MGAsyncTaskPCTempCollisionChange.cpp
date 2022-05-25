// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/MGAsyncTaskPCTempCollisionChange.h"

UMGAsyncTaskPCTempCollisionChange* UMGAsyncTaskPCTempCollisionChange::TempCollisionChange(const UObject* WorldContextObject, UPrimitiveComponent* PrimitiveComponent,
                                                                                          float ForDuration)
{
	UMGAsyncTaskPCTempCollisionChange* BlueprintNode = NewObject<UMGAsyncTaskPCTempCollisionChange>();
	BlueprintNode->PrimitiveComponent = PrimitiveComponent;
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->ForDuration = ForDuration;

	return BlueprintNode;
}

void UMGAsyncTaskPCTempCollisionChange::Activate()
{
	if (nullptr == WorldContextObject)
	{
		FFrame::KismetExecutionMessage(TEXT("Invalid WorldContextObject. Cannot execute UMGAsyncTaskPCTempCollisionChange."), ELogVerbosity::Error);
		return;
	}

	if (PrimitiveComponent)
		PrimitiveComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	WorldContextObject->GetWorld()->GetTimerManager().SetTimer(Timer, this, &UMGAsyncTaskPCTempCollisionChange::DelayFinished, ForDuration);
}

void UMGAsyncTaskPCTempCollisionChange::DelayFinished()
{
	WorldContextObject->GetWorld()->GetTimerManager().ClearTimer(Timer);
	Timer.Invalidate();
	if (PrimitiveComponent)
		PrimitiveComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	OnTempCollisionChangeEnd.Broadcast();
	SetReadyToDestroy();
	MarkAsGarbage();
}
