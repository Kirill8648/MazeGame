// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MGMazeGenerationGameMode.generated.h"

DECLARE_DELEGATE_TwoParams(FString2Delegate, FString, FString)
DECLARE_DELEGATE(FDelegate)

UCLASS()
class MAZEGAME_API AMGMazeGenerationGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AMGMazeGenerationGameMode();
	
	FString2Delegate ChangeLoadingScreenTextDelegate;

	FDelegate AllFinishedDelegate;

	UPROPERTY(EditAnywhere, Category = "MGMazeGenerationGameMode|Widget")
	TSubclassOf<UUserWidget> LoadingScreenWidget;

	UPROPERTY(BlueprintReadOnly, Category = "MGMazeGenerationGameMode|Widget")
	UUserWidget* LoadingScreenWidgetRef;

	UFUNCTION(BlueprintImplementableEvent)
	void K2_UpdateLoadingScreenWidgetText(FString const& TableNamespace, FString  const& KeyInTable);

protected:
	virtual bool ReadyToStartMatch_Implementation() override;

	virtual void BeginPlay() override;

	virtual void OnMatchStateSet() override;

private:
	bool bIsReadyToStart = false;
	
	void AllFinished();

	void UpdateLoadingScreenWidgetTextHelperFunction(FString TableNamespace, FString KeyInTable);
};
