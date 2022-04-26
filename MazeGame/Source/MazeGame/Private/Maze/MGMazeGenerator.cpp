// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/MGMazeGenerator.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Maze/MGInstancedMeshActor.h"

AMGMazeGenerator::AMGMazeGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	MatrixGenerationFinishedDelegate.BindUObject(this, &AMGMazeGenerator::ContinueGeneration);
	DrawGenerationProgressUIDelegate.BindUObject(this, &AMGMazeGenerator::DrawGenerationProgressUI);
}

void AMGMazeGenerator::LaunchAsyncMazeGeneration(int32 Seed1, int32 XSize, int32 YSize, FString2Delegate& ChangeLoadingScreenTextDelegate1, FDelegate& AllFinishedDelegate1)
{
	this->Seed = Seed1;
	this->ChangeLoadingScreenTextDelegate = ChangeLoadingScreenTextDelegate1;
	this->AllFinishedDelegate = AllFinishedDelegate1;

	MazeGenerationProgressWidgetRef = Cast<UMGMazeGenerationProgressWidget>(CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), MazeGenerationProgressWidget));

	if (MazeGenerationProgressWidgetRef)
	{
		MazeGenerationProgressWidgetRef->AddToViewport(1);
	}
	(new FAutoDeleteAsyncTask<FGenerateMazeMatrixAsyncTask>(MatrixGenerationFinishedDelegate, DrawGenerationProgressUIDelegate, MazeMatrix, Seed, YSize, XSize))->
		StartBackgroundTask();
}

FVector AMGMazeGenerator::GetPlayerStartCoords()
{
	FVector Returning;

	for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
	{
		for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
		{
			if (MazeMatrix[IndexY][IndexX].Distance == 1)
			{
				Returning = FVector(IndexY * DistanceBetweenWalls / 2, IndexX * DistanceBetweenWalls / 2, 250.0f);
			}
		}
	}

	return Returning;
}

/*
void AMGMazeGenerator::SimpleSpawnGeneratedMaze()
{
	for (auto& MazeItemY : MazeMatrix)
	{
		for (const auto& MazeItemX : MazeItemY)
		{
			if (MazeItemX.MazeItemState == VerticalEdge)
			{
				if (MazeItemX.bIsUnbreakable == true)

					GetWorld()->SpawnActor<AActor>(UnbreakableMazeWall, FVector(MazeItemX.IndexX * 500 / 2, MazeItemX.IndexY * 500 / 2, 0.0f), FRotator(0.0f, 90.0f, 0.0f));
				else
					GetWorld()->SpawnActor<AActor>(CommonMazeWall, FVector(MazeItemX.IndexX * 500 / 2, MazeItemX.IndexY * 500 / 2, 0.0f), FRotator(0.0f, 90.0f, 0.0f));
			}
			else if (MazeItemX.MazeItemState == HorizontalEdge)
			{
				if (MazeItemX.bIsUnbreakable == true)
					GetWorld()->SpawnActor<AActor>(UnbreakableMazeWall, FVector(MazeItemX.IndexX * 500 / 2, MazeItemX.IndexY * 500 / 2, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
				else
					GetWorld()->SpawnActor<AActor>(CommonMazeWall, FVector(MazeItemX.IndexX * 500 / 2, MazeItemX.IndexY * 500 / 2, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
			}
		}
	}

	const MazeItem* FurthestRoom = &MazeMatrix[0][0];
	for (auto& MazeItemY : MazeMatrix)
		for (auto& MazeItemX : MazeItemY)
			if (MazeItemX.Distance > FurthestRoom->Distance)
				FurthestRoom = &MazeItemX;

	UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->TeleportTo(FVector(FurthestRoom->IndexX, FurthestRoom->IndexY, 0), FRotator(), false, true);
}

void AMGMazeGenerator::SpawnWithInstances()
{
	if (InstancedMeshCommonWalls && InstancedMeshUnbreakableWalls)
	{
		AMGInstancedMeshActor* InstancedMeshCommonWallsActor = GetWorld()->SpawnActor<AMGInstancedMeshActor>(
			InstancedMeshCommonWalls, FVector(0.0f, 0.0f, 250.0f), FRotator(0.0f, 0.0f, 0.0f));
		AMGInstancedMeshActor* InstancedMeshUnbreakableWallsActor = GetWorld()->SpawnActor<AMGInstancedMeshActor>(
			InstancedMeshUnbreakableWalls, FVector(0.0f, 0.0f, 250.0f), FRotator(0.0f, 0.0f, 0.0f));

		InstancedMeshCommonWallsActor->ReplacementActorZOffset = -250.0f;
		InstancedMeshUnbreakableWallsActor->ReplacementActorZOffset = -250.0f;

		for (auto& MazeItemY : MazeMatrix)
		{
			for (const auto& MazeItemX : MazeItemY)
			{
				if (MazeItemX.MazeItemState == VerticalEdge)
				{
					if (MazeItemX.bIsUnbreakable == true)
						InstancedMeshUnbreakableWallsActor->HierarchicalInstancedMesh->AddInstance(
							FTransform(FRotator(0.0f, 90.0f, 0.0f), FVector(MazeItemX.IndexX * 500 / 2, MazeItemX.IndexY * 500 / 2, 0.0f)));
					else
						InstancedMeshCommonWallsActor->HierarchicalInstancedMesh->AddInstance(
							FTransform(FRotator(0.0f, 90.0f, 0.0f), FVector(MazeItemX.IndexX * 500 / 2, MazeItemX.IndexY * 500 / 2, 0.0f)));
				}
				else if (MazeItemX.MazeItemState == HorizontalEdge)
				{
					if (MazeItemX.bIsUnbreakable == true)
						InstancedMeshUnbreakableWallsActor->HierarchicalInstancedMesh->AddInstance(
							FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(MazeItemX.IndexX * 500 / 2, MazeItemX.IndexY * 500 / 2, 0.0f)));
					else
						InstancedMeshCommonWallsActor->HierarchicalInstancedMesh->AddInstance(
							FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(MazeItemX.IndexX * 500 / 2, MazeItemX.IndexY * 500 / 2, 0.0f)));
				}
			}
		}
	}
}
*/

void AMGMazeGenerator::BeginPlay()
{
	Super::BeginPlay();

	/*if (MazeGenerationProgressWidgetRef)
	{
		MazeGenerationProgressWidgetRef->AddToViewport(1);
	}*/

	//LaunchAsyncMazeGeneration(MGSeed, MGXSize, MGYSize);

	/*int32 SecondsAtBeginning;
	float PartialSecondsAtBeginning;
	int32 SecondsAfter;
	float PartialSecondsAfter;


	UGameplayStatics::GetAccurateRealTime(SecondsAtBeginning, PartialSecondsAtBeginning);
	GenerateMazeWithSeed(MGSeed, MGYSize, MGXSize);
	UGameplayStatics::GetAccurateRealTime(SecondsAfter, PartialSecondsAfter);
	float PastTime = SecondsAfter - SecondsAtBeginning + PartialSecondsAfter - PartialSecondsAtBeginning;
	GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, FString::Printf(TEXT("The maze was generated in %f seconds"), PastTime));

	PrintMazeMatrixToLog();
	if (bIsMistakeHappened && GEngine)
		GEngine->AddOnScreenDebugMessage(-1, INFINITY, FColor::Red, FString::Printf(TEXT("Errors occurred during generation! Check the log for more details.")));

	UGameplayStatics::GetAccurateRealTime(SecondsAtBeginning, PartialSecondsAtBeginning);
	//SimpleSpawnGeneratedMaze();
	SpawnWithInstances();
	UGameplayStatics::GetAccurateRealTime(SecondsAfter, PartialSecondsAfter);
	PastTime = SecondsAfter - SecondsAtBeginning + PartialSecondsAfter - PartialSecondsAtBeginning;
	GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, FString::Printf(TEXT("The maze was spawned in %f seconds"), PastTime));
*/
}

void AMGMazeGenerator::PrintMazeMatrixToLog()
{
	for (auto& MazeItemY : MazeMatrix)
	{
		FString Line;
		for (const auto& MazeItemX : MazeItemY)
		{
			switch (MazeItemX.MazeItemState)
			{
			case Cell: Line.AppendChar('#');
				break;
			case HorizontalEdge: Line.AppendChar('-');
				break;
			case VerticalEdge: Line.AppendChar('|');
				break;
			case Knot: Line.AppendChar('+');
				break;
			default:
				Line.AppendChar(' ');
			}
		}
		UE_LOG(LogTemp, Display, TEXT("%s"), *Line);
	}
}

void AMGMazeGenerator::ContinueGeneration(bool bIsMistakeHappenedInAsync)
{
	PrintMazeMatrixToLog();

	if (bIsMistakeHappenedInAsync && GEngine)
		GEngine->AddOnScreenDebugMessage(-1, INFINITY, FColor::Red, FString::Printf(TEXT("Errors occurred during async generation! Check the log for more details.")));
	else
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green, FString::Printf(TEXT("The maze matrix was successfully async generated")));

	FFunctionGraphTask::CreateAndDispatchWhenReady([this]()
	{
		ChangeLoadingScreenTextDelegate.ExecuteIfBound("Text", "ObjectsGeneration");
	}, TStatId(), nullptr, ENamedThreads::GameThread);

	//ChangeLoadingScreenTextDelegate1.ExecuteIfBound("Text", "ObjectsGeneration");

	FString Text;
	for (int i = 1; i < MazeMatrix.Num(); i = i + 2)
	{
		for (int j = 1; j < MazeMatrix[i].Num(); j = j + 2)
		{
			if (MazeMatrix[i][j].bIsVisited)Text.AppendChar('#');
			else Text.AppendChar('+');
		}
		Text.Append("\n");
	}

	FFunctionGraphTask::CreateAndDispatchWhenReady([this, Text]()
	{
		DrawGenerationProgressUIDelegate.ExecuteIfBound(Text);
	}, TStatId(), nullptr, ENamedThreads::GameThread);
	//DrawGenerationProgressUIDelegate.ExecuteIfBound(Text);

	//TODO Objects Generation

	FFunctionGraphTask::CreateAndDispatchWhenReady([this]()
	{
		ChangeLoadingScreenTextDelegate.ExecuteIfBound("Text", "ObjectPlacement");
		SpawnObjects();

		//SpawnWithInstances();
	}, TStatId(), nullptr, ENamedThreads::GameThread);
	//ChangeLoadingScreenTextDelegate1.ExecuteIfBound("Text", "ObjectPlacement");
	//SpawnWithInstances();
	//SpawnObjects();

	//Cast<AMGMazeGenerationGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->RestartPlayer(UGameplayStatics::GetPlayerController(GetWorld(),0));

	//MazeGenerationProgressWidgetRef->RemoveFromParent();
	FFunctionGraphTask::CreateAndDispatchWhenReady([this]()
	{
		MazeGenerationProgressWidgetRef->RemoveFromParent();
		AllFinishedDelegate.ExecuteIfBound();
	}, TStatId(), nullptr, ENamedThreads::GameThread);
	//AllFinishedDelegate1.ExecuteIfBound();
}

void AMGMazeGenerator::DrawGenerationProgressUI(FString StringToDisplay)
{
	FText Text = FText::FromString(StringToDisplay);

	FFunctionGraphTask::CreateAndDispatchWhenReady([this,Text]()
	{
		MazeGenerationProgressWidgetRef->TextBlock->SetText(Text);
	}, TStatId(), nullptr, ENamedThreads::GameThread);
}

void AMGMazeGenerator::SpawnObjects()
{
	FRandomStream Stream(Seed);

	int32 x = MazeMatrix.Num();
	int32 y = MazeMatrix[0].Num();
	int32 CellCount = (x - 1) / 2 * ((y - 1) / 2);
	int32 KnotCount = (y - 3) / 2 * ((x - 3) / 2);

	//-----SPAWN UNBREAKABLE WALLS-----
	AMGInstancedMeshActorStatic* InstancedMeshUnbreakableWallsActor = GetWorld()->SpawnActor<AMGInstancedMeshActorStatic>(
		InstancedMeshUnbreakableWalls, FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
	for (auto& MazeItemY : MazeMatrix)
	{
		for (auto& MazeItemX : MazeItemY)
		{
			if (MazeItemX.MazeItemState == VerticalEdge)
			{
				if (MazeItemX.bIsUnbreakable == true)
				{
					InstancedMeshUnbreakableWallsActor->HierarchicalInstancedMesh->AddInstance(
						FTransform(FRotator(0.0f, 90.0f, 0.0f), FVector(MazeItemX.IndexX * DistanceBetweenWalls / 2, MazeItemX.IndexY * DistanceBetweenWalls / 2, 0.0f)));
					MazeItemX.MazeItemState = None;
				}
			}
			else if (MazeItemX.MazeItemState == HorizontalEdge)
			{
				if (MazeItemX.bIsUnbreakable == true)
				{
					InstancedMeshUnbreakableWallsActor->HierarchicalInstancedMesh->AddInstance(
						FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(MazeItemX.IndexX * DistanceBetweenWalls / 2, MazeItemX.IndexY * DistanceBetweenWalls / 2, 0.0f)));
					MazeItemX.MazeItemState = None;
				}
			}
		}
	}
	//--------------------------------


	//-----------SPAWN CELLS-----------
	//каждый из массивов перемешать и разделить на куски(проценты), которые написаны в входных массивах
	//итерация сквозь все клекти должна включать: отдельные акторы, собираемые предметы
	//TODO добавить проверку соотношения клеток (не больше единицы в целом)
	TArray<int32> CellIndexes;
	CellIndexes.SetNum(CellCount);
	for (int32 i = 0; i < CellCount; i++)
		CellIndexes[i] = i;
	ShuffleArray(CellIndexes, Stream);
	int32 CellAmountCorrectionAppliedToCoins = 0;

	for (FSeparateSpawnedActorInfo SeparateActorInfo : SeparateActors)
	{
		if (SeparateActorInfo.ActorType == Cell)
		{
			TArray<int32> ActorIndexesToSpawn;
			if (!SeparateActorInfo.bIsSpawnRateAsCount)
			{
				int32 ActorAmount = FMath::RoundHalfToZero(SeparateActorInfo.SpawnRate * CellCount);
				int32 Temp = CellIndexes.Num();
				for (int32 i = Temp - 1; i > Temp - 1 - ActorAmount /*&& i >= 0*/; i--)
				{
					ActorIndexesToSpawn.Emplace(CellIndexes[i]);
					CellIndexes.RemoveAt(i);
				}
			}
			else
			{
				CellAmountCorrectionAppliedToCoins += SeparateActorInfo.SpawnRate;
				int32 ActorAmount = SeparateActorInfo.SpawnRate;
				int32 Temp = CellIndexes.Num();
				for (int32 i = Temp - 1; i > Temp - 1 - ActorAmount /*&& i >= 0*/; i--)
				{
					ActorIndexesToSpawn.Emplace(CellIndexes[i]);
					CellIndexes.RemoveAt(i);
				}
			}
			int32 Counter = 0;
			for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
			{
				for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
				{
					if (MazeMatrix[IndexY][IndexX].MazeItemState == Cell)
					{
						if (ActorIndexesToSpawn.Contains(Counter))
						{
							GetWorld()->SpawnActor<AActor>(SeparateActorInfo.SpawnedActor.GetDefaultObject()->GetClass(),
							                               FTransform(SeparateActorInfo.OptionalRotation,
							                                          FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2, 0.0f) + SeparateActorInfo.
							                                          OptionalOffset, FVector(1.0f, 1.0f, 1.0f)));
						}
						Counter++;
					}
				}
			}
		}
	}

	for (FInstancedCollectibleInfo InstancedCollectible : InstancedCollectibles)
	{
		TArray<int32> CollectibleIndexesToSpawn;
		int32 CollectibleAmount = FMath::RoundHalfToZero(InstancedCollectible.SpawnRate * CellCount);
		int32 Temp = CellIndexes.Num();
		for (int32 i = Temp - 1; i > Temp - 1 - CollectibleAmount /*&& i >= 0*/; i--)
		{
			CollectibleIndexesToSpawn.Emplace(CellIndexes[i]);
			CellIndexes.RemoveAt(i);
		}
		AMGInstancedMeshActorStatic* InstancedMeshCollectibleActor = GetWorld()->SpawnActor<AMGInstancedMeshActorStatic>(
			InstancedCollectible.InstancedActor, InstancedCollectible.OptionalOffset, FRotator(0.0f, 0.0f, 0.0f));
		int32 Counter = 0;
		for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
		{
			for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
			{
				if (MazeMatrix[IndexY][IndexX].MazeItemState == Cell)
				{
					if (CollectibleIndexesToSpawn.Contains(Counter))
					{
						InstancedMeshCollectibleActor->HierarchicalInstancedMesh->AddInstance(
							FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2, 0.0f)));
					}
					Counter++;
				}
			}
		}
	}

	TArray<int32> AbilitiesIndexesToSpawn;
	CellAmountCorrectionAppliedToCoins += NumOfSpawnedAbilityCollectibleActor;
	int32 Temp2 = CellIndexes.Num();
	for (int32 i = Temp2 - 1; i > Temp2 - 1 - NumOfSpawnedAbilityCollectibleActor /*&& i >= 0*/; i--)
	{
		AbilitiesIndexesToSpawn.Emplace(CellIndexes[i]);
		CellIndexes.RemoveAt(i);
	}
	int32 Counter2;
	for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
	{
		for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
		{
			if (MazeMatrix[IndexY][IndexX].MazeItemState == Cell)
			{
				if (AbilitiesIndexesToSpawn.Contains(Counter2))
				{
					GetWorld()->SpawnActor<AActor>(AbilityCollectibleActor.GetDefaultObject()->GetClass(),
					                               FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2, 0.0f),
					                                          FVector(1.0f, 1.0f, 1.0f)));
				}
				Counter2++;
			}
		}
	}

	TArray<int32> CoinsIndexesToSpawn;
	int32 CoinsAmount = FMath::RoundHalfToZero(CoinsSpawnRate * CellCount) - CellAmountCorrectionAppliedToCoins;
	int32 Temp = CellIndexes.Num();
	for (int32 i = Temp - 1; i > Temp - 1 - CoinsAmount /*&& i >= 0*/; i--)
	{
		CoinsIndexesToSpawn.Emplace(CellIndexes[i]);
		CellIndexes.RemoveAt(i);
	}
	AMGInstancedCollectibleActor* InstancedMeshCoinsActor = GetWorld()->SpawnActor<AMGInstancedCollectibleActor>(
		InstancedMeshCoins, OptionalInstancedMeshCoinsOffset, FRotator(0.0f, 0.0f, 0.0f));
	int32 Counter = 0;
	for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
	{
		for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
		{
			if (MazeMatrix[IndexY][IndexX].MazeItemState == Cell)
			{
				if (CoinsIndexesToSpawn.Contains(Counter))
				{
					InstancedMeshCoinsActor->HierarchicalInstancedMesh->AddInstance(
						FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2, 0.0f)));
				}
				Counter++;
			}
		}
	}
	//--------------------------------


	int32 VEdgeCount = 0; /* = (y - 3) / 2 * (x - 2);*/
	int32 HEdgeCount = 0; /* = (x - 3) / 2 * (y - 2);*/
	for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
		for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
			if (MazeMatrix[IndexY][IndexX].MazeItemState == HorizontalEdge) HEdgeCount++;
			else if (MazeMatrix[IndexY][IndexX].MazeItemState == VerticalEdge) VEdgeCount++;


	//-----------SPAWN EDGES----------
	//каждый из массивов перемешать и разделить на куски(проценты), которые написаны в входных массивах
	//итерация сквозь все грани должна включать: отдельные акторы, инстанс объекты стены
	//TODO добавить проверку соотношения граней (не больше единицы в целом)
	TArray<int32> HEdgeIndexes;
	HEdgeIndexes.SetNum(HEdgeCount);
	for (int32 i = 0; i < HEdgeCount; i++)
		HEdgeIndexes[i] = i;
	ShuffleArray(HEdgeIndexes, Stream);
	int32 HEdgeAmountCorrectionApplied = 0;
	TArray<int32> VEdgeIndexes;
	VEdgeIndexes.SetNum(VEdgeCount);
	for (int32 i = 0; i < VEdgeCount; i++)
		VEdgeIndexes[i] = i;
	ShuffleArray(VEdgeIndexes, Stream);
	int32 VEdgeAmountCorrectionApplied = 0;
	for (FSeparateSpawnedActorInfo SeparateActorInfo : SeparateActors)
	{
		if (SeparateActorInfo.ActorType == HorizontalEdge)
		{
			TArray<int32> ActorIndexesToSpawn;
			if (!SeparateActorInfo.bIsSpawnRateAsCount)
			{
				int32 ActorAmount = FMath::RoundHalfToZero(SeparateActorInfo.SpawnRate * HEdgeCount);
				int32 Temp1 = HEdgeIndexes.Num();
				for (int32 i = Temp1 - 1; i > Temp1 - 1 - ActorAmount /*&& i >= 0*/; i--)
				{
					ActorIndexesToSpawn.Emplace(HEdgeIndexes[i]);
					HEdgeIndexes.RemoveAt(i);
				}
			}
			else
			{
				HEdgeAmountCorrectionApplied += SeparateActorInfo.SpawnRate;
				int32 ActorAmount = SeparateActorInfo.SpawnRate;
				int32 Temp1 = HEdgeIndexes.Num();
				for (int32 i = Temp1 - 1; i > Temp1 - 1 - ActorAmount /*&& i >= 0*/; i--)
				{
					ActorIndexesToSpawn.Emplace(HEdgeIndexes[i]);
					HEdgeIndexes.RemoveAt(i);
				}
			}
			int32 Counter1 = 0;
			for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
			{
				for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
				{
					if (MazeMatrix[IndexY][IndexX].MazeItemState == HorizontalEdge)
					{
						if (ActorIndexesToSpawn.Contains(Counter1))
						{
							GetWorld()->SpawnActor<AActor>(SeparateActorInfo.SpawnedActor.GetDefaultObject()->GetClass(),
							                               FTransform(SeparateActorInfo.OptionalRotation,
							                                          FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2, 0.0f) + SeparateActorInfo.
							                                          OptionalOffset, FVector(1.0f, 1.0f, 1.0f)));
						}
						Counter1++;
					}
				}
			}
		}
		else if (SeparateActorInfo.ActorType == VerticalEdge)
		{
			TArray<int32> ActorIndexesToSpawn;
			if (!SeparateActorInfo.bIsSpawnRateAsCount)
			{
				int32 ActorAmount = FMath::RoundToInt32(SeparateActorInfo.SpawnRate * VEdgeCount);
				int32 Temp1 = VEdgeIndexes.Num();
				for (int32 i = Temp1 - 1; i > Temp1 - 1 - ActorAmount /*&& i >= 0*/; i--)
				{
					ActorIndexesToSpawn.Emplace(VEdgeIndexes[i]);
					VEdgeIndexes.RemoveAt(i);
				}
			}
			else
			{
				VEdgeAmountCorrectionApplied += SeparateActorInfo.SpawnRate;
				int32 ActorAmount = SeparateActorInfo.SpawnRate;
				int32 Temp1 = VEdgeIndexes.Num();
				for (int32 i = Temp1 - 1; i > Temp1 - 1 - ActorAmount /*&& i >= 0*/; i--)
				{
					ActorIndexesToSpawn.Emplace(VEdgeIndexes[i]);
					VEdgeIndexes.RemoveAt(i);
				}
			}
			int32 Counter1 = 0;
			for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
			{
				for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
				{
					if (MazeMatrix[IndexY][IndexX].MazeItemState == VerticalEdge)
					{
						if (ActorIndexesToSpawn.Contains(Counter1))
						{
							GetWorld()->SpawnActor<AActor>(SeparateActorInfo.SpawnedActor.GetDefaultObject()->GetClass(),
							                               FTransform(SeparateActorInfo.OptionalRotation,
							                                          FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2, 0.0f) + SeparateActorInfo.
							                                          OptionalOffset, FVector(1.0f, 1.0f, 1.0f)));
						}
						Counter1++;
					}
				}
			}
		}
	}
	for (int32 i = 0; i < InstancedWallsVariations.Num(); i++)
	{
		TArray<int32> HWallsIndexesToSpawn;
		TArray<int32> VWallsIndexesToSpawn;
		int32 HWallsAmount;
		int32 VWallsAmount;
		if (i == 0)
		{
			HWallsAmount = FMath::RoundHalfToZero(InstancedWallsVariations[i].HorizontalSpawnRate * HEdgeCount) - HEdgeAmountCorrectionApplied;
			VWallsAmount = FMath::RoundHalfToZero(InstancedWallsVariations[i].VerticalSpawnRate * VEdgeCount) - VEdgeAmountCorrectionApplied;
		}
		else
		{
			HWallsAmount = FMath::RoundHalfToZero(InstancedWallsVariations[i].HorizontalSpawnRate * HEdgeCount);
			VWallsAmount = FMath::RoundHalfToZero(InstancedWallsVariations[i].VerticalSpawnRate * VEdgeCount);
		}

		int32 Temp1 = HEdgeIndexes.Num();
		for (int32 j = Temp1 - 1; j > Temp1 - 1 - HWallsAmount && j >= 0; j--)
		{
			HWallsIndexesToSpawn.Emplace(HEdgeIndexes[j]);
			HEdgeIndexes.RemoveAt(j);
		}
		Temp1 = VEdgeIndexes.Num();
		for (int32 j = Temp1 - 1; j > Temp1 - 1 - VWallsAmount && j >= 0; j--)
		{
			VWallsIndexesToSpawn.Emplace(VEdgeIndexes[j]);
			VEdgeIndexes.RemoveAt(j);
		}

		AMGInstancedMeshActorStatic* InstancedMeshWallActor = GetWorld()->SpawnActor<AMGInstancedMeshActorStatic>(
			InstancedWallsVariations[i].InstancedActor, InstancedWallsVariations[i].OptionalOffset, FRotator(0.0f, 0.0f, 0.0f));

		int32 HorizontalCounter = 0;
		int32 VerticalCounter = 0;
		for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
		{
			for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
			{
				if (MazeMatrix[IndexY][IndexX].MazeItemState == HorizontalEdge)
				{
					if (HWallsIndexesToSpawn.Contains(HorizontalCounter))
					{
						InstancedMeshWallActor->HierarchicalInstancedMesh->AddInstance(
							FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2, 0.0f)));
					}
					HorizontalCounter++;
				}
				else if (MazeMatrix[IndexY][IndexX].MazeItemState == VerticalEdge)
				{
					if (VWallsIndexesToSpawn.Contains(VerticalCounter))
					{
						InstancedMeshWallActor->HierarchicalInstancedMesh->AddInstance(
							FTransform(FRotator(0.0f, 90.0f, 0.0f), FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2, 0.0f)));
					}
					VerticalCounter++;
				}
			}
		}
	}
	//--------------------------------


	//----------SPAWN FLOORS----------
	TArray<int32> FloorIndexes;
	FloorIndexes.SetNum(CellCount);
	for (int32 i = 0; i < CellCount; i++)
		FloorIndexes[i] = i;
	ShuffleArray(FloorIndexes, Stream);
	int32 FloorAmountCorrectionApplied = 0;

	//TODO TArray<TArray<bool>> isFloorSpawned;

	for (FSeparateSpawnedFloorActorInfo SeparateActorInfo : FloorActors)
	{
		TArray<int32> FloorIndexesToSpawn;
		if (!SeparateActorInfo.bIsSpawnRateAsCount)
		{
			int32 ActorAmount = FMath::RoundHalfToZero(SeparateActorInfo.SpawnRate * CellCount);
			int32 Temp1 = FloorIndexes.Num();
			for (int32 i = Temp1 - 1; i > Temp1 - 1 - ActorAmount /*&& i >= 0*/; i--)
			{
				FloorIndexesToSpawn.Emplace(FloorIndexes[i]);
				FloorIndexes.RemoveAt(i);
			}
		}
		else
		{
			FloorAmountCorrectionApplied += SeparateActorInfo.SpawnRate;
			int32 ActorAmount = SeparateActorInfo.SpawnRate;
			int32 Temp1 = FloorIndexes.Num();
			for (int32 i = Temp1 - 1; i > Temp1 - 1 - ActorAmount /*&& i >= 0*/; i--)
			{
				FloorIndexesToSpawn.Emplace(FloorIndexes[i]);
				FloorIndexes.RemoveAt(i);
			}
		}
		int32 Counter1 = 0;
		for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
		{
			for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
			{
				if (MazeMatrix[IndexY][IndexX].MazeItemState == Cell)
				{
					if (FloorIndexesToSpawn.Contains(Counter1))
					{
						GetWorld()->SpawnActor<AActor>(SeparateActorInfo.SpawnedFloorActor.GetDefaultObject()->GetClass(),
						                               FTransform(SeparateActorInfo.OptionalRotation,
						                                          FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2, 0.0f) + SeparateActorInfo.
						                                          OptionalOffset, FVector(1.0f, 1.0f, 1.0f)));
					}
					Counter1++;
				}
			}
		}
	}

	for (int32 i = 0; i < InstancedFloorVariations.Num(); i++)
	{
		TArray<int32> FloorIndexesToSpawn;
		int32 FloorAmount;
		if (i == 0) FloorAmount = FMath::RoundHalfToZero(InstancedFloorVariations[i].SpawnRate * CellCount) - FloorAmountCorrectionApplied;
		else FloorAmount = FMath::RoundHalfToZero(InstancedFloorVariations[i].SpawnRate * CellCount);

		int32 Temp1 = FloorIndexes.Num();
		for (int32 j = Temp1 - 1; j > Temp1 - 1 - FloorAmount /*&& j >= 0*/; j--)
		{
			FloorIndexesToSpawn.Emplace(FloorIndexes[j]);
			FloorIndexes.RemoveAt(j);
		}

		AMGInstancedMeshActorStatic* InstancedFloorWallActor = GetWorld()->SpawnActor<AMGInstancedMeshActorStatic>(
			InstancedFloorVariations[i].InstancedActor, InstancedFloorVariations[i].OptionalOffset, FRotator(0.0f, 0.0f, 0.0f));

		int32 Counter1 = 0;
		for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
		{
			for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
			{
				if (MazeMatrix[IndexY][IndexX].MazeItemState == Cell)
				{
					if (FloorIndexesToSpawn.Contains(Counter1))
					{
						InstancedFloorWallActor->HierarchicalInstancedMesh->AddInstance(
							FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2, 0.0f)));
					}
					Counter1++;
				}
			}
		}
	}
	//--------------------------------


	//TODO добавить генерацию узлов, разделить инстанс меши
	//потом TArray<int32> KnotIndexes;

	/*
	for (FSeparateSpawnedActorInfo SeparateActorInfo : SeparateActors)
	{
		if (SeparateActorInfo.bIsSpawnRateAsCount)
		{
			for (int32 i = 0; i < SeparateActorInfo.SpawnRate; i++)
			{
				bool bIsSpawned = false;
				do
				{
					int32 RandomPlace;
					switch (SeparateActorInfo.ActorType)
					{
					case Cell:
						RandomPlace = Stream.RandRange(1, CellCount);
						break;
					case VerticalEdge:
						RandomPlace = Stream.RandRange(1, VEdgeCount);
						break;
					case HorizontalEdge:
						RandomPlace = Stream.RandRange(1, HEdgeCount);
						break;
					case Knot:
						RandomPlace = Stream.RandRange(1, KnotCount);
						break;
					default:
						GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "SepActors not spawned properly, None enum encountered in SeparateActors array");
						return;
					}
	
					int32 Counter = 1;
					for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
					{
						for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
						{
							if (MazeMatrix[IndexY][IndexX].MazeItemState == SeparateActorInfo.ActorType)
							{
								if (Counter == RandomPlace)
								{
									/*GetWorld()->SpawnActorDeferred<AActor>(SeparateActorInfo.SpawnedActor.GetDefaultObject()->GetClass(),
									                                       FTransform(FRotator(0, 0, 0),
									                                                  FVector(IndexX * 500 / 2, IndexY * 500 / 2, 0.0f) + SeparateActorInfo.OptionalOffset));#1#
									/*GetWorld()->SpawnActor<AActor>(SeparateActorInfo.SpawnedActor.GetDefaultObject()->GetClass(),
																									   FVector(IndexX * 500 / 2, IndexY * 500 / 2, 0.0f) + SeparateActorInfo.OptionalOffset,
																									   SeparateActorInfo.OptionalRotation);#1#
									//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "SpawnedObject");
									GetWorld()->SpawnActorAbsolute(SeparateActorInfo.SpawnedActor.GetDefaultObject()->GetClass(),
									                               FTransform(FRotator(0.0f, 0.0f, 0.0f),
									                                          FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2, 0.0f) +
									                                          SeparateActorInfo.OptionalOffset, FVector(1.0f, 1.0f, 1.0f)));
									switch (SeparateActorInfo.ActorType)
									{
									case Cell:
										CellCount--;
										break;
									case VerticalEdge:
										VEdgeCount--;
										break;
									case HorizontalEdge:
										HEdgeCount--;
										break;
									case Knot:
										KnotCount--;
										break;
									default:
										GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red,
										                                 "SepActors not spawned properly, None enum encountered in SeparateActors array");
										return;
									}
									MazeMatrix[IndexY][IndexX].MazeItemState = None;
									bIsSpawned = true;
								}
								Counter++;
							}
						}
					}
				}
				while (!bIsSpawned);
			}
		}
		else
		{
			int32 ActorsAmount;
			switch (SeparateActorInfo.ActorType)
			{
			case Cell:
				ActorsAmount = SeparateActorInfo.SpawnRate * CellCount;
				break;
			case VerticalEdge:
				ActorsAmount = SeparateActorInfo.SpawnRate * VEdgeCount;
				break;
			case HorizontalEdge:
				ActorsAmount = SeparateActorInfo.SpawnRate * HEdgeCount;
				break;
			case Knot:
				ActorsAmount = SeparateActorInfo.SpawnRate * KnotCount;
				break;
			default:
				GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "SepActors not spawned properly, None enum encountered in SeparateActors array");
				return;
			}
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Need to spawn %d actors"), ActorsAmount));
			for (int32 i = 0; i < ActorsAmount; i++)
			{
				bool bIsSpawned = false;
				do
				{
					int32 RandomPlace;
					switch (SeparateActorInfo.ActorType)
					{
					case Cell:
						RandomPlace = Stream.RandRange(1, CellCount);
						break;
					case VerticalEdge:
						RandomPlace = Stream.RandRange(1, VEdgeCount);
						break;
					case HorizontalEdge:
						RandomPlace = Stream.RandRange(1, HEdgeCount);
						break;
					case Knot:
						RandomPlace = Stream.RandRange(1, KnotCount);
						break;
					default:
						GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "SepActors not spawned properly, None enum encountered in SeparateActors array");
						return;
					}
	
					int32 Counter = 1;
					for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
					{
						for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
						{
							if (MazeMatrix[IndexY][IndexX].MazeItemState == SeparateActorInfo.ActorType)
							{
								if (Counter == RandomPlace)
								{
									GetWorld()->SpawnActorAbsolute(SeparateActorInfo.SpawnedActor.GetDefaultObject()->GetClass(),
									                               FTransform(FRotator(0.0f, 0.0f, 0.0f),
									                                          FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2, 0.0f) +
									                                          SeparateActorInfo.OptionalOffset, FVector(1.0f, 1.0f, 1.0f)));
									switch (SeparateActorInfo.ActorType)
									{
									case Cell:
										CellCount--;
										break;
									case VerticalEdge:
										VEdgeCount--;
										break;
									case HorizontalEdge:
										HEdgeCount--;
										break;
									case Knot:
										KnotCount--;
										break;
									default:
										GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red,
										                                 "SepActors not spawned properly, None enum encountered in SeparateActors array");
										return;
									}
									MazeMatrix[IndexY][IndexX].MazeItemState = None;
									bIsSpawned = true;
								}
								Counter++;
							}
						}
					}
				}
				while (!bIsSpawned);
			}
		}
	}
	
	//---spawn walls---------
	
	for (FInstancedActorInfo InstancedWallVariation : InstancedWallsVariations)
	{
		AMGInstancedMeshActor* InstancedMeshCommonWallsActor = GetWorld()->SpawnActor<AMGInstancedMeshActor>(
			InstancedWallVariation.InstancedActor, InstancedWallVariation.OptionalOffset, FRotator(0.0f, 0.0f, 0.0f));
	
		int32 VerticalAmount = VEdgeCount * InstancedWallVariation.SpawnRate;
		int32 HorizontalAmount = HEdgeCount * InstancedWallVariation.SpawnRate;
	
		/*for (int32 i = 0; i < VerticalAmount; i++)
		{
			bool bIsSpawned = false;
			do
			{
				int32 RandomPlace = Stream.RandRange(1, VEdgeCount);
	
				int32 Counter = 1;
				for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
				{
					for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
					{
						if (MazeMatrix[IndexY][IndexX].MazeItemState == VerticalEdge)
						{
							if (Counter == RandomPlace)
							{
								InstancedMeshCommonWallsActor->HierarchicalInstancedMesh->AddInstance(FTransform(
									FRotator(0.0f, 90.0f, 0.0f),
									FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2, 0.0f) + InstancedWallVariation.OptionalOffset));
	
								VEdgeCount--;
								MazeMatrix[IndexY][IndexX].MazeItemState = None;
								bIsSpawned = true;
							}
							Counter++;
						}
					}
				}
			}
			while (!bIsSpawned);
		}#1#
	
		TArray<int32> RandomHEdgeIndexes;
		RandomHEdgeIndexes.SetNum(HEdgeCount);
		for (int32 i = 0; i < HEdgeCount; i++)
		{
			RandomHEdgeIndexes[i] = i;
		}
		ShuffleArray(RandomHEdgeIndexes);
		for (int32 i = RandomHEdgeIndexes.Num() - 1; i > HorizontalAmount - 1; i--)
		{
			RandomHEdgeIndexes.RemoveAt/*Swap#1#(i);
		}
		int32 Counter = 0;
		for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
		{
			for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
			{
				if (MazeMatrix[IndexY][IndexX].MazeItemState == HorizontalEdge)
				{
					if (RandomHEdgeIndexes.Contains(Counter))
					{
						InstancedMeshCommonWallsActor->HierarchicalInstancedMesh->AddInstance(FTransform(
							FRotator(0.0f, 0.0f, 0.0f),
							FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2, 0.0f) + InstancedWallVariation.OptionalOffset));
	
						//HEdgeCount--;
						MazeMatrix[IndexY][IndexX].MazeItemState = None;
					}
					Counter++;
				}
			}
		}
	
		TArray<int32> RandomVEdgeIndexes;
		RandomVEdgeIndexes.SetNum(VEdgeCount);
		for (int32 i = 0; i < VEdgeCount; i++)
		{
			RandomVEdgeIndexes[i] = i;
		}
		ShuffleArray(RandomVEdgeIndexes);
		for (int32 i = RandomVEdgeIndexes.Num() - 1; i > VerticalAmount - 1; i--)
		{
			RandomVEdgeIndexes.RemoveAtSwap(i);
		}
		Counter = 0;
		for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
		{
			for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
			{
				if (MazeMatrix[IndexY][IndexX].MazeItemState == VerticalEdge)
				{
					if (RandomVEdgeIndexes.Contains(Counter))
					{
						InstancedMeshCommonWallsActor->HierarchicalInstancedMesh->AddInstance(FTransform(
							FRotator(0.0f, 90.0f, 0.0f),
							FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2, 0.0f) + InstancedWallVariation.OptionalOffset));
	
						//VEdgeCount--;
						MazeMatrix[IndexY][IndexX].MazeItemState = None;
					}
					Counter++;
				}
			}
		}
	
		/*for (int32 i = 0; i < HorizontalAmount; i++)
		{
			bool bIsSpawned = false;
			do
			{
				int32 RandomPlace = Stream.RandRange(1, HEdgeCount);
	
				int32 Counter = 1;
				for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
				{
					for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
					{
						if (MazeMatrix[IndexY][IndexX].MazeItemState == HorizontalEdge)
						{
							if (Counter == RandomPlace)
							{
								InstancedMeshCommonWallsActor->HierarchicalInstancedMesh->AddInstance(FTransform(
									FRotator(0.0f, 0.0f, 0.0f),
									FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2, 0.0f) + InstancedWallVariation.OptionalOffset));
	
								HEdgeCount--;
								MazeMatrix[IndexY][IndexX].MazeItemState = None;
								bIsSpawned = true;
							}
							Counter++;
						}
					}
				}
			}
			while (!bIsSpawned);
		}#1#
	}
	*/
}

void AMGMazeGenerator::ShuffleArray(TArray<int32>& Array, FRandomStream& Stream)
{
	if (Array.Num() > 0)
	{
		int32 LastIndex = Array.Num() - 1;
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			int32 Index = Stream.RandRange(i, LastIndex); /*FMath::RandRange(i, LastIndex);*/
			if (i != Index)
			{
				Array.Swap(i, Index);
			}
		}
	}
}

void FGenerateMazeMatrixAsyncTask::DoWork()
{
	const FRandomStream Stream(Seed);

	MatrixAddress.Empty();
	MatrixAddress.SetNum(YSize * 2 + 1);
	for (auto& MazeItemY : MatrixAddress)
		MazeItemY.SetNum(XSize * 2 + 1);

	MatrixAddress[0][0].MazeItemState = Knot;
	MatrixAddress[0][0].bIsUnbreakable = true;
	MatrixAddress[0][MatrixAddress[0].Num() - 1].MazeItemState = Knot;
	MatrixAddress[0][MatrixAddress[0].Num() - 1].bIsUnbreakable = true;
	MatrixAddress[MatrixAddress.Num() - 1][0].MazeItemState = Knot;
	MatrixAddress[MatrixAddress.Num() - 1][0].bIsUnbreakable = true;
	MatrixAddress[MatrixAddress.Num() - 1][MatrixAddress[0].Num() - 1].MazeItemState = Knot;
	MatrixAddress[MatrixAddress.Num() - 1][MatrixAddress[0].Num() - 1].bIsUnbreakable = true;

	const int32 Exit = Stream.RandRange(0, 2 * (YSize + XSize) - 1);
	const MazeItem* ExitCell = nullptr;
	int32 EdgesIter = 0;
	for (int32 IndexY = 0; IndexY != MatrixAddress.Num(); ++IndexY)
	{
		for (int32 IndexX = 0; IndexX != MatrixAddress[IndexY].Num(); ++IndexX)
		{
			MatrixAddress[IndexY][IndexX].IndexX = IndexX;
			MatrixAddress[IndexY][IndexX].IndexY = IndexY;
			if (IndexX % 2 == 0 && IndexY % 2 == 0)
			{
				MatrixAddress[IndexY][IndexX].MazeItemState = Knot;
				if (IndexY == 0 || IndexY == MatrixAddress.Num() - 1 || IndexX == 0 || IndexX == MatrixAddress[IndexY].Num() - 1)
					MatrixAddress[IndexY][IndexX].bIsUnbreakable = true;
			}
			else if ((IndexY == 0 || IndexY == MatrixAddress.Num() - 1) && !MatrixAddress[IndexY][IndexX].bIsUnbreakable)
			{
				if (EdgesIter != Exit)
				{
					MatrixAddress[IndexY][IndexX].MazeItemState = HorizontalEdge;
					MatrixAddress[IndexY][IndexX].bIsUnbreakable = true;
				}
				else ExitCell = &MatrixAddress[IndexY][IndexX];
				EdgesIter++;
			}
			else if ((IndexX == 0 || IndexX == MatrixAddress[IndexY].Num() - 1) && !MatrixAddress[IndexY][IndexX].bIsUnbreakable)
			{
				if (EdgesIter != Exit)
				{
					MatrixAddress[IndexY][IndexX].MazeItemState = VerticalEdge;
					MatrixAddress[IndexY][IndexX].bIsUnbreakable = true;
				}
				else ExitCell = &MatrixAddress[IndexY][IndexX];
				EdgesIter++;
			}
			else if (IndexX % 2 == 1 && IndexY % 2 == 1)
			{
				MatrixAddress[IndexY][IndexX].MazeItemState = Cell;
			}
		}
	}

	if (MatrixAddress.Num() < 2)
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Warning, TEXT("The MazeMatrix is not generated. Either input values are 0 or BeginPlay() called too early for seed %d"), 12345);
		return;
	}
	if (ExitCell == nullptr)
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Error, TEXT("The exit is not generated for some reason for seed %d"), 12345);
		bIsMistakeHappenedInAsync = true;
		return;
	}
	TArray<MazeItem*> DesiredCells;
	DesiredCells.Emplace(GetNearCellsByIndexes(ExitCell->IndexY, ExitCell->IndexX)[0]);
	DesiredCells[0]->Distance = 1;


	int32 SecondsAtBeginning;
	float PartialSecondsAtBeginning;
	int32 SecondsAfter;
	float PartialSecondsAfter;

	UGameplayStatics::GetAccurateRealTime(SecondsAtBeginning, PartialSecondsAtBeginning);

	do
	{
		UGameplayStatics::GetAccurateRealTime(SecondsAfter, PartialSecondsAfter);
		const float PastTime = SecondsAfter - SecondsAtBeginning + PartialSecondsAfter - PartialSecondsAtBeginning;

		if (PastTime >= 0.0000001f/*0.00000001f*//*0.000001f*//*0.00000000001f*/)
		{
			FString Text;

			for (int i = 1; i < MatrixAddress.Num(); i = i + 2)
			{
				for (int j = 1; j < MatrixAddress[i].Num(); j = j + 2)
				{
					if (MatrixAddress[i][j].bIsVisited)Text.AppendChar('#');
					else Text.AppendChar(' ');
				}
				Text.Append("\n");
			}
			DrawUIDelegate.ExecuteIfBound(Text);
		}

		MazeItem* CurrentCell = DesiredCells[Stream.RandRange(0, DesiredCells.Num() - 1)];
		CurrentCell->bIsVisited = true;

		TArray<MazeItem*> IterationNearCells;
		IterationNearCells.Append(GetNearCellsByIndexes(CurrentCell->IndexY, CurrentCell->IndexX));
		for (int32 i = IterationNearCells.Num() - 1; i >= 0; i--)
			if (IterationNearCells[i]->bIsVisited) IterationNearCells.RemoveAt(i);

		const int32 R = Stream.RandRange(0, IterationNearCells.Num());
		for (int32 i = R; i > 1; i--)
		{
			const int32 IterRand = Stream.RandRange(0, i - 1);
			IterationNearCells[IterRand]->Distance = CurrentCell->Distance + 1;
			DesiredCells.Emplace(IterationNearCells[IterRand]);
			IterationNearCells.RemoveAt(IterRand);
		}

		for (const auto& IterationNearRemainingCell : IterationNearCells)
		{
			PlaceWallBetweenCellsByIndexes(IterationNearRemainingCell->IndexY, IterationNearRemainingCell->IndexX, CurrentCell->IndexY, CurrentCell->IndexX);
		}

		DesiredCells.Remove(CurrentCell);

		if (DesiredCells.IsEmpty())
		{
			MazeItem* NotVisitedCell = nullptr;
			int32 MaxDistance = -1;
			for (auto& MazeItemY : MatrixAddress)
				for (auto& MazeItemX : MazeItemY)
					if (MazeItemX.bIsVisited != true && MazeItemX.MazeItemState == Cell)
						for (const auto& NearCell : GetNearCellsByIndexes(MazeItemX.IndexY, MazeItemX.IndexX, true))
							if (NearCell->bIsVisited == true && NearCell->Distance > MaxDistance)
							{
								MaxDistance = NearCell->Distance;
								NotVisitedCell = &MazeItemX;
								break;
							}

			if (NotVisitedCell)
			{
				TArray<MazeItem*> NearVisitedCellsForNotVisitedCell = GetNearCellsByIndexes(NotVisitedCell->IndexY, NotVisitedCell->IndexX, true);
				for (int32 i = NearVisitedCellsForNotVisitedCell.Num() - 1; i >= 0; i--)
					if (!NearVisitedCellsForNotVisitedCell[i]->bIsVisited) NearVisitedCellsForNotVisitedCell.RemoveAt(i);

				if (NearVisitedCellsForNotVisitedCell.Num() > 0)
				{
					const int32 RandomizedNearVisitedCellsForNotVisitedCellIndex = Stream.RandRange(0, NearVisitedCellsForNotVisitedCell.Num() - 1);

					BreakWallBetweenCellsByIndexes(NotVisitedCell->IndexY, NotVisitedCell->IndexX,
					                               NearVisitedCellsForNotVisitedCell[RandomizedNearVisitedCellsForNotVisitedCellIndex]->IndexY,
					                               NearVisitedCellsForNotVisitedCell[RandomizedNearVisitedCellsForNotVisitedCellIndex]->IndexX);
				}
				DesiredCells.Emplace(NotVisitedCell);
			}
		}
		UGameplayStatics::GetAccurateRealTime(SecondsAtBeginning, PartialSecondsAtBeginning);
	}
	while (!DesiredCells.IsEmpty());

	EndDelegate.ExecuteIfBound(bIsMistakeHappenedInAsync);
}

TArray<MazeItem*> FGenerateMazeMatrixAsyncTask::GetNearCellsByIndexes(int32 IndexY, int32 IndexX, bool bIgnoreWalls)
{
	TArray<MazeItem*> Returning;

	if (IndexY > MatrixAddress.Num() - 1 || IndexX > MatrixAddress[0].Num() || IndexY < 0 || IndexX < 0)
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Error, TEXT("Index out of bounds in function GetNearCellsByIndexes(%d, %d) with seed %d"), IndexY, IndexX, 12345);
		bIsMistakeHappenedInAsync = true;
		return Returning;
	}

	if (MatrixAddress[IndexY][IndexX].MazeItemState == Cell)
	{
		if (IndexY - 2 >= 0)
			if (MatrixAddress[IndexY - 2][IndexX].MazeItemState == Cell)
				if (bIgnoreWalls)
					Returning.Emplace(&MatrixAddress[IndexY - 2][IndexX]);
				else if (MatrixAddress[IndexY - 1][IndexX].MazeItemState != HorizontalEdge)
					Returning.Emplace(&MatrixAddress[IndexY - 2][IndexX]);
		if (IndexX - 2 >= 0)
			if (MatrixAddress[IndexY][IndexX - 2].MazeItemState == Cell)
				if (bIgnoreWalls)
					Returning.Emplace(&MatrixAddress[IndexY][IndexX - 2]);
				else if (MatrixAddress[IndexY][IndexX - 1].MazeItemState != VerticalEdge)
					Returning.Emplace(&MatrixAddress[IndexY][IndexX - 2]);
		if (IndexY + 2 <= MatrixAddress.Num() - 2)
			if (MatrixAddress[IndexY + 2][IndexX].MazeItemState == Cell)
				if (bIgnoreWalls)
					Returning.Emplace(&MatrixAddress[IndexY + 2][IndexX]);
				else if (MatrixAddress[IndexY + 1][IndexX].MazeItemState != HorizontalEdge)
					Returning.Emplace(&MatrixAddress[IndexY + 2][IndexX]);
		if (IndexX + 2 <= MatrixAddress[0].Num() - 2)
			if (MatrixAddress[IndexY][IndexX + 2].MazeItemState == Cell)
				if (bIgnoreWalls)
					Returning.Emplace(&MatrixAddress[IndexY][IndexX + 2]);
				else if (MatrixAddress[IndexY][IndexX + 1].MazeItemState != VerticalEdge)
					Returning.Emplace(&MatrixAddress[IndexY][IndexX + 2]);
	}
	else if (MatrixAddress[IndexY][IndexX].MazeItemState == HorizontalEdge || MatrixAddress[IndexY][IndexX].MazeItemState == VerticalEdge || MatrixAddress[IndexY][IndexX].
		MazeItemState
		== None)
	{
		if (IndexY - 1 >= 0)
			if (MatrixAddress[IndexY - 1][IndexX].MazeItemState == Cell)
				Returning.Emplace(&MatrixAddress[IndexY - 1][IndexX]);
		if (IndexX - 1 >= 0)
			if (MatrixAddress[IndexY][IndexX - 1].MazeItemState == Cell)
				Returning.Emplace(&MatrixAddress[IndexY][IndexX - 1]);
		if (IndexY + 1 <= MatrixAddress.Num() - 1)
			if (MatrixAddress[IndexY + 1][IndexX].MazeItemState == Cell)
				Returning.Emplace(&MatrixAddress[IndexY + 1][IndexX]);
		if (IndexX + 1 <= MatrixAddress[0].Num() - 1)
			if (MatrixAddress[IndexY][IndexX + 1].MazeItemState == Cell)
				Returning.Emplace(&MatrixAddress[IndexY][IndexX + 1]);
	}
	else
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Error, TEXT("Trying to get near cells not for Edge, Cell or None in indexes Y:%d X:%d with seed %d"), IndexY, IndexX, 12345);
		bIsMistakeHappenedInAsync = true;
	}
	return Returning;
}

void FGenerateMazeMatrixAsyncTask::PlaceWallBetweenCellsByIndexes(int32 IndexY1, int32 IndexX1, int32 IndexY2, int32 IndexX2)
{
	if (MatrixAddress[IndexY1][IndexX1].MazeItemState != Cell || MatrixAddress[IndexY2][IndexX2].MazeItemState != Cell)
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Error, TEXT("Trying to place walls not between cells in PlaceWallBetweenCellsByIndexes(%d, %d, %d, %d) with seed %d"), IndexY1, IndexX1,
		       IndexY2, IndexX2, 12345);
		bIsMistakeHappenedInAsync = true;
		return;
	}

	if (IndexY1 > IndexY2 && IndexX1 == IndexX2)
		MatrixAddress[IndexY1 - 1][IndexX1].MazeItemState = HorizontalEdge;
	else if (IndexY1 < IndexY2 && IndexX1 == IndexX2)
		MatrixAddress[IndexY1 + 1][IndexX1].MazeItemState = HorizontalEdge;
	else if (IndexX1 > IndexX2 && IndexY1 == IndexY2)
		MatrixAddress[IndexY1][IndexX1 - 1].MazeItemState = VerticalEdge;
	else if (IndexX1 < IndexX2 && IndexY1 == IndexY2)
		MatrixAddress[IndexY1][IndexX1 + 1].MazeItemState = VerticalEdge;
	else
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Error, TEXT("The wall was not placed between the cells in PlaceWallBetweenCellsByIndexes(%d, %d, %d, %d) with seed %d"), IndexY1,
		       IndexX1, IndexY2, IndexX2, 12345);
		bIsMistakeHappenedInAsync = true;
	}
}

void FGenerateMazeMatrixAsyncTask::BreakWallBetweenCellsByIndexes(int32 IndexY1, int32 IndexX1, int32 IndexY2, int32 IndexX2)
{
	if (MatrixAddress[IndexY1][IndexX1].MazeItemState != Cell || MatrixAddress[IndexY2][IndexX2].MazeItemState != Cell)
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Error, TEXT("Trying to break walls not between cells in BreakWallBetweenCellsByIndexes(%d, %d, %d, %d) with seed %d"), IndexY1, IndexX1,
		       IndexY2, IndexX2, 12345);
		bIsMistakeHappenedInAsync = true;
		return;
	}
	if (IndexY1 > IndexY2 && IndexX1 == IndexX2)
		MatrixAddress[IndexY1 - 1][IndexX1].MazeItemState = None;
	else if (IndexY1 < IndexY2 && IndexX1 == IndexX2)
		MatrixAddress[IndexY1 + 1][IndexX1].MazeItemState = None;
	else if (IndexX1 > IndexX2 && IndexY1 == IndexY2)
		MatrixAddress[IndexY1][IndexX1 - 1].MazeItemState = None;
	else if (IndexX1 < IndexX2 && IndexY1 == IndexY2)
		MatrixAddress[IndexY1][IndexX1 + 1].MazeItemState = None;
	else
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Error, TEXT("The wall was not broken between the cells in BreakWallBetweenCellsByIndexes(%d, %d, %d, %d) with seed %d"), IndexY1,
		       IndexX1, IndexY2, IndexX2, 12345);
		bIsMistakeHappenedInAsync = true;
	}
}
