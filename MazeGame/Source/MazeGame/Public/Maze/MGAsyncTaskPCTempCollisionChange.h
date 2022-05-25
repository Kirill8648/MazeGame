// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "MGAsyncTaskPCTempCollisionChange.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTempCollisionChangeFinished);

/**
 * 
 */
UCLASS()
class MAZEGAME_API UMGAsyncTaskPCTempCollisionChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FOnTempCollisionChangeFinished OnTempCollisionChangeEnd;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "MazeGame|AsyncTask")
	static UMGAsyncTaskPCTempCollisionChange* TempCollisionChange(const UObject* WorldContextObject, UPrimitiveComponent* PrimitiveComponent, float ForDuration);

protected:
	UPROPERTY()
	UPrimitiveComponent* PrimitiveComponent;
	void DelayFinished();

	virtual void Activate() override;

private:
	float ForDuration;
	FTimerHandle Timer;
	const UObject* WorldContextObject;
};
