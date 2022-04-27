// Fill out your copyright notice in the Description page of Project Settings.


#include "MGMazeGenerationGameMode.h"

#include "MGPlayerDataSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Maze/MGMazeGenerator.h"

AMGMazeGenerationGameMode::AMGMazeGenerationGameMode()
{
	ChangeLoadingScreenTextDelegate.BindUObject(this, &AMGMazeGenerationGameMode::UpdateLoadingScreenWidgetTextHelperFunction);
	AllFinishedDelegate.BindUObject(this, &AMGMazeGenerationGameMode::AllFinished);
}

bool AMGMazeGenerationGameMode::ReadyToStartMatch_Implementation()
{
	return bIsReadyToStart;
}

void AMGMazeGenerationGameMode::BeginPlay()
{
	Super::BeginPlay();
	/*LoadingScreenWidgetRef = CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), LoadingScreenWidget);
	UpdateLoadingScreenWidgetText("Text", "MazeGeneration");


	TArray<AActor*> Generators;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMGMazeGenerator::StaticClass(), Generators);

	AMGMazeGenerator* Generator = nullptr;
	if (!Generators.IsEmpty())
	{
		Generator = Cast<AMGMazeGenerator>(Generators[0]);
	}

	const UMGPlayerDataSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMGPlayerDataSubsystem>();
	if (Generator)
	{
		Generator->LaunchAsyncMazeGeneration(Subsystem->CurrentRunSeed, Subsystem->CurrentlyLoadedSaveGameObject->LastXMazeSize,
		                                     Subsystem->CurrentlyLoadedSaveGameObject->LastYMazeSize, ChangeLoadingScreenTextDelegate, AllFinishedDelegate);
	}
	else GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "MazeGenerator Actor not found on this map");*/
}

void AMGMazeGenerationGameMode::OnMatchStateSet()
{
	if (MatchState == MatchState::WaitingToStart)
	{
		UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UMGPlayerDataSubsystem>()->CurrentlyLoadedSaveGameObject->SpawnedAbilityCollectiblesCache.Empty();
		LoadingScreenWidgetRef = CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), LoadingScreenWidget);
		LoadingScreenWidgetRef->AddToViewport(0);
		K2_UpdateLoadingScreenWidgetText("Text", "MazeGeneration");

		TArray<AActor*> Generators;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMGMazeGenerator::StaticClass(), Generators);

		AMGMazeGenerator* Generator = nullptr;
		if (!Generators.IsEmpty())
		{
			Generator = Cast<AMGMazeGenerator>(Generators[0]);
		}

		const UMGPlayerDataSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMGPlayerDataSubsystem>();
		if (Generator && Subsystem)
		{
			Generator->LaunchAsyncMazeGeneration(Subsystem->CurrentRunSeed, Subsystem->CurrentlyLoadedSaveGameObject->LastXMazeSize,
			                                     Subsystem->CurrentlyLoadedSaveGameObject->LastYMazeSize, ChangeLoadingScreenTextDelegate, AllFinishedDelegate);
		}
		else GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "MazeGenerator Actor not found on this map");
	} /*else if (MatchState==MatchState::InProgress)
	{
		RestartPlayer(UGameplayStatics::GetPlayerController(GetWorld(),0));
	}*/
	Super::OnMatchStateSet();
}

void AMGMazeGenerationGameMode::AllFinished()
{
	TArray<AActor*> Generators;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMGMazeGenerator::StaticClass(), Generators);

	AMGMazeGenerator* Generator = nullptr;
	if (!Generators.IsEmpty())
	{
		Generator = Cast<AMGMazeGenerator>(Generators[0]);
	}

	if (Generator)
	{
		GetWorld()->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), Generator->GetPlayerStartCoords(), FRotator());
	}

	LoadingScreenWidgetRef->RemoveFromParent();
	bIsReadyToStart = true;
}

void AMGMazeGenerationGameMode::UpdateLoadingScreenWidgetTextHelperFunction(FString TableNamespace, FString KeyInTable)
{
	K2_UpdateLoadingScreenWidgetText(TableNamespace, KeyInTable);
}
