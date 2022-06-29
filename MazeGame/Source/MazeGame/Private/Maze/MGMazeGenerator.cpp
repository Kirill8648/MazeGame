// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/MGMazeGenerator.h"

#include "EditorLevelUtils.h"
#include "ImageUtils.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Maze/MGInstancedMeshActor.h"
#include "Maze/Generation/MGBiomeData.h"
#include "Maze/Generation/MGMazeStateSubsystem.h"

AMGMazeGenerator::AMGMazeGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

	MatrixGenerationFinishedDelegate.BindUObject(this, &AMGMazeGenerator::ContinueGeneration);
	DrawGenerationProgressUIDelegate.BindUObject(this, &AMGMazeGenerator::DrawGenerationProgressUI);
}

void AMGMazeGenerator::LaunchAsyncMazeGeneration(int32 Seed1, int32 XSize, int32 YSize,
                                                 FString2Delegate& ChangeLoadingScreenTextDelegate1,
                                                 FDelegate& AllFinishedDelegate1)
{
	this->Seed = Seed1;
	this->ChangeLoadingScreenTextDelegate = ChangeLoadingScreenTextDelegate1;
	this->AllFinishedDelegate = AllFinishedDelegate1;

	MazeGenerationProgressWidgetRef = Cast<UMGMazeGenerationProgressWidget>(
		CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), MazeGenerationProgressWidget));

	if (MazeGenerationProgressWidgetRef)
	{
		MazeGenerationProgressWidgetRef->AddToViewport(1);
	}
	(new FAutoDeleteAsyncTask<FGenerateMazeMatrixAsyncTask>(MatrixGenerationFinishedDelegate,
	                                                        DrawGenerationProgressUIDelegate, MazeMatrix, ExitCell,
	                                                        Seed, YSize, XSize))->
		StartBackgroundTask();
}

FVector AMGMazeGenerator::GetPlayerStartCoords()
{
	int32 Max = -10000;

	FVector Returning;

	for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
	{
		for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
		{
			if (MazeMatrix[IndexY][IndexX].Distance > Max)
			{
				Returning = FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2, 250.0f);
				Max = MazeMatrix[IndexY][IndexX].Distance;
			}
		}
	}

	return Returning;
}

void AMGMazeGenerator::BeginPlay()
{
	Super::BeginPlay();
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
		GEngine->AddOnScreenDebugMessage(-1, INFINITY, FColor::Red,
		                                 FString::Printf(TEXT(
			                                 "Errors occurred during async generation! Check the log for more details.")));
	else
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green,
		                                 FString::Printf(TEXT("The maze matrix was successfully async generated")));

	FFunctionGraphTask::CreateAndDispatchWhenReady([this]()
	{
		ChangeLoadingScreenTextDelegate.ExecuteIfBound("Text", "ObjectsGeneration");
	}, TStatId(), nullptr, ENamedThreads::GameThread);

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

	FFunctionGraphTask::CreateAndDispatchWhenReady([this]()
	{
		ChangeLoadingScreenTextDelegate.ExecuteIfBound("Text", "ObjectPlacement");
		SpawnObjects();
	}, TStatId(), nullptr, ENamedThreads::GameThread);

	FFunctionGraphTask::CreateAndDispatchWhenReady([this]()
	{
		MazeGenerationProgressWidgetRef->RemoveFromParent();
		AllFinishedDelegate.ExecuteIfBound();
	}, TStatId(), nullptr, ENamedThreads::GameThread);
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
	AMGInstancedMeshActorStatic* InstancedMeshUnbreakableWallsActor = GetWorld()->SpawnActor<
		AMGInstancedMeshActorStatic>(
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
						FTransform(FRotator(0.0f, 90.0f, 0.0f),
						           FVector(MazeItemX.IndexX * DistanceBetweenWalls / 2,
						                   MazeItemX.IndexY * DistanceBetweenWalls / 2, 0.0f),
						           FVector(1.25f, 1.25f, 1.0f)));
					MazeItemX.MazeItemState = None;
				}
			}
			else if (MazeItemX.MazeItemState == HorizontalEdge)
			{
				if (MazeItemX.bIsUnbreakable == true)
				{
					InstancedMeshUnbreakableWallsActor->HierarchicalInstancedMesh->AddInstance(
						FTransform(FRotator(0.0f, 0.0f, 0.0f),
						           FVector(MazeItemX.IndexX * DistanceBetweenWalls / 2,
						                   MazeItemX.IndexY * DistanceBetweenWalls / 2, 0.0f),
						           FVector(1.25f, 1.25f, 1.0f)));
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

	TArray<int32> AbilitiesIndexesToSpawn;
	CellAmountCorrectionAppliedToCoins += NumOfSpawnedAbilityCollectibleActor;
	int32 Temp2 = CellIndexes.Num();
	for (int32 i = Temp2 - 1; i > Temp2 - 1 - NumOfSpawnedAbilityCollectibleActor /*&& i >= 0*/; i--)
	{
		AbilitiesIndexesToSpawn.Emplace(CellIndexes[i]);
		CellIndexes.RemoveAt(i);
	}
	int32 Counter2 = 0;
	for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
	{
		for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
		{
			if (MazeMatrix[IndexY][IndexX].MazeItemState == Cell)
			{
				if (AbilitiesIndexesToSpawn.Contains(Counter2))
				{
					GetWorld()->SpawnActor<AActor>(AbilityCollectibleActor.GetDefaultObject()->GetClass(),
					                               FTransform(FRotator(0.0f, 0.0f, 0.0f),
					                                          FVector(IndexX * DistanceBetweenWalls / 2,
					                                                  IndexY * DistanceBetweenWalls / 2, 0.0f),
					                                          FVector(1.0f, 1.0f, 1.0f)));
				}
				Counter2++;
			}
		}
	}

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
							GetWorld()->SpawnActor<AActor>(
								SeparateActorInfo.SpawnedActor.GetDefaultObject()->GetClass(),
								FTransform(SeparateActorInfo.OptionalRotation,
								           FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2,
								                   0.0f) + SeparateActorInfo.
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
		for (int32 i = Temp - 1; i > Temp - 1 - CollectibleAmount && i >= 0; i--)
		{
			CollectibleIndexesToSpawn.Emplace(CellIndexes[i]);
			CellIndexes.RemoveAt(i);
		}
		AMGInstancedMeshActorStatic* InstancedMeshCollectibleActor = GetWorld()->SpawnActor<
			AMGInstancedMeshActorStatic>(
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
							FTransform(FRotator(0.0f, 0.0f, 0.0f),
							           FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2,
							                   0.0f)));
					}
					Counter++;
				}
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
						FTransform(FRotator(0.0f, 0.0f, 0.0f),
						           FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2,
						                   0.0f)));
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
	//каждый из массивов перемешать и разделить на куски(проценты), которые написаны во входных массивах
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
							GetWorld()->SpawnActor<AActor>(
								SeparateActorInfo.SpawnedActor.GetDefaultObject()->GetClass(),
								FTransform(SeparateActorInfo.OptionalRotation,
								           FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2,
								                   0.0f) + SeparateActorInfo.
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
							GetWorld()->SpawnActor<AActor>(
								SeparateActorInfo.SpawnedActor.GetDefaultObject()->GetClass(),
								FTransform(SeparateActorInfo.OptionalRotation,
								           FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2,
								                   0.0f) + SeparateActorInfo.
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
			HWallsAmount = FMath::RoundHalfToZero(InstancedWallsVariations[i].HorizontalSpawnRate * HEdgeCount) -
				HEdgeAmountCorrectionApplied;
			VWallsAmount = FMath::RoundHalfToZero(InstancedWallsVariations[i].VerticalSpawnRate * VEdgeCount) -
				VEdgeAmountCorrectionApplied;
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
			InstancedWallsVariations[i].InstancedActor, InstancedWallsVariations[i].OptionalOffset,
			FRotator(0.0f, 0.0f, 0.0f));

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
							FTransform(FRotator(0.0f, 0.0f, 0.0f),
							           FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2,
							                   0.0f)));
					}
					HorizontalCounter++;
				}
				else if (MazeMatrix[IndexY][IndexX].MazeItemState == VerticalEdge)
				{
					if (VWallsIndexesToSpawn.Contains(VerticalCounter))
					{
						InstancedMeshWallActor->HierarchicalInstancedMesh->AddInstance(
							FTransform(FRotator(0.0f, 90.0f, 0.0f),
							           FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2,
							                   0.0f)));
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
			for (int32 i = Temp1 - 1; i > Temp1 - 1 - ActorAmount && i >= 0; i--)
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
			for (int32 i = Temp1 - 1; i > Temp1 - 1 - ActorAmount && i >= 0; i--)
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
						GetWorld()->SpawnActor<AActor>(
							SeparateActorInfo.SpawnedFloorActor.GetDefaultObject()->GetClass(),
							FTransform(SeparateActorInfo.OptionalRotation,
							           FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2,
							                   0.0f) + SeparateActorInfo.
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
		if (i == 0)
			FloorAmount = FMath::RoundHalfToZero(InstancedFloorVariations[i].SpawnRate * CellCount) -
				FloorAmountCorrectionApplied;
		else FloorAmount = FMath::RoundHalfToZero(InstancedFloorVariations[i].SpawnRate * CellCount);

		int32 Temp1 = FloorIndexes.Num();
		for (int32 j = Temp1 - 1; j > Temp1 - 1 - FloorAmount /*&& j >= 0*/; j--)
		{
			FloorIndexesToSpawn.Emplace(FloorIndexes[j]);
			FloorIndexes.RemoveAt(j);
		}

		AMGInstancedMeshActorStatic* InstancedFloorWallActor = GetWorld()->SpawnActor<AMGInstancedMeshActorStatic>(
			InstancedFloorVariations[i].InstancedActor, InstancedFloorVariations[i].OptionalOffset,
			FRotator(0.0f, 0.0f, 0.0f));

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
							FTransform(FRotator(0.0f, 0.0f, 0.0f),
							           FVector(IndexX * DistanceBetweenWalls / 2, IndexY * DistanceBetweenWalls / 2,
							                   0.0f)));
					}
					Counter1++;
				}
			}
		}
	}
	//--------------------------------

	//-------SPAWN EXIT VOLUME--------
	for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
	{
		for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
		{
			if (MazeMatrix[IndexY][IndexX].IndexX == ExitCell.IndexX && MazeMatrix[IndexY][IndexX].IndexY == ExitCell.
				IndexY)
			{
				FVector ExitVolumeLocation;
				if (ExitCell.IndexY == 0)
					ExitVolumeLocation = FVector(ExitCell.IndexX * DistanceBetweenWalls / 2,
					                             ExitCell.IndexY * DistanceBetweenWalls / 2 - DistanceBetweenWalls,
					                             0.0f);
				else if (ExitCell.IndexY == MazeMatrix.Num() - 1)
					ExitVolumeLocation = FVector(ExitCell.IndexX * DistanceBetweenWalls / 2,
					                             ExitCell.IndexY * DistanceBetweenWalls / 2 + DistanceBetweenWalls,
					                             0.0f);
				else if (ExitCell.IndexX == 0)
					ExitVolumeLocation = FVector(ExitCell.IndexX * DistanceBetweenWalls / 2 - DistanceBetweenWalls,
					                             ExitCell.IndexY * DistanceBetweenWalls / 2, 0.0f);
				else if (ExitCell.IndexX == MazeMatrix[0].Num() - 1)
					ExitVolumeLocation = FVector(ExitCell.IndexX * DistanceBetweenWalls / 2 + DistanceBetweenWalls,
					                             ExitCell.IndexY * DistanceBetweenWalls / 2, 0.0f);
				GetWorld()->SpawnActor<AActor>(ExitLevelVolume, ExitVolumeLocation, FRotator(0.0f, 0.0f, 0.0f));
			}
		}
	}
	//--------------------------------
}

template <typename InElementType, typename InAllocatorType>
void AMGMazeGenerator::ShuffleArray(TArray<InElementType, InAllocatorType>& Array, FRandomStream& Stream)
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

TArray<UTexture2D*> AMGMazeGenerator::GenerateBiomes()
{
	TArray<UTexture2D*> Returnable;

	FRandomStream PerlinNoiseStream(Seed);
	PerlinReset(PerlinNoiseStream);
	MazeMatrix.SetNum(100);
	for (auto& MazeItemY : MazeMatrix)
		MazeItemY.SetNum(100);

	//-------проверки-------
	if (Biomes.Num() == 0)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "No Biomes added! Generation cancelled");
		UE_LOG(LogTemp, Error, TEXT("No Biomes added! Generation cancelled"));
		return Returnable;
	}
	for (const UMGBiomeData* BiomeData : Biomes)
		if (!BiomeData)
		{
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "There is an empty Biome element in the array! Generation cancelled");
			UE_LOG(LogTemp, Error, TEXT("There is an empty Biome element in the array! Generation cancelled"));
			return Returnable;
		}
	for (int32 i = 0; i < Biomes.Num(); i++)
		for (int32 j = 0; j < Biomes.Num(); j++)
			if ((Biomes[i]->BiomeName.IsEqual(Biomes[j]->BiomeName) || Biomes[i] == Biomes[j]) && i != j)
			{
				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "There are duplicate Biomes or names of Biomes! Generation cancelled");
				UE_LOG(LogTemp, Error, TEXT("There are duplicate Biomes or names of Biomes! Generation cancelled"));
				return Returnable;
			}
	UMGMazeStateSubsystem* MazeStateSubsystem = GetWorld()->GetSubsystem<UMGMazeStateSubsystem>();
	if (!MazeStateSubsystem)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "Failed to get UMGMazeStateSubsystem! Generation cancelled");
		UE_LOG(LogTemp, Error, TEXT("Failed to get UMGMazeStateSubsystem! Generation cancelled"));
		return Returnable;
	}
	if (!HeightRedistribution)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, "No HeightRedistribution selected, using linear redistribution");
		UE_LOG(LogTemp, Warning, TEXT("No HeightRedistribution selected, using linear redistribution"));
	}
	if (!HeatRedistribution)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, "No HeatRedistribution selected, using linear redistribution");
		UE_LOG(LogTemp, Warning, TEXT("No HeatRedistribution selected, using linear redistribution"));
	}
	if (!MoistureRedistribution)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, "No MoistureRedistribution selected, using linear redistribution");
		UE_LOG(LogTemp, Warning, TEXT("No MoistureRedistribution selected, using linear redistribution"));
	}
	for (auto Biome : Biomes)
	{
		float SpawnRateSum = 0.0f;
		for (auto Room : Biome->Rooms)
			if (Room.SpawnType == SpawnRate)
				SpawnRateSum += Room.SpawnRate;
		if (SpawnRateSum > 1.0f)
		{
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT(
					                                 "SpawnRate sum with SpawnType=SpawnRate of biome '%s' are more than 1.0f! Generation cancelled"),
				                                                                         *Biome->BiomeName.ToString()));
			UE_LOG(LogTemp, Error, TEXT("SpawnRate sum with SpawnType=SpawnRate of biome '%s' are more than 1.0f! Generation cancelled"),
			       *Biome->BiomeName.ToString());
			return Returnable;
		}
	}
	//----------------------

	for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
		for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
			MazeStateSubsystem->RoomsStates.Add(UMGMazeStateSubsystem::GetRoomHash(this, IndexX * 250.0f, IndexY * 250.0f),
			                                    FRoomState(FVector2d(IndexX * 250.0f, IndexY * 250.0f)));
	//сгенерировать карту высоты, теплоты и влажности
	for (auto& RoomState : MazeStateSubsystem->RoomsStates)
	{
		float nx = RoomState.Value.GridCoords.X / MazeMatrix[0].Num() - 0.5f;
		float ny = RoomState.Value.GridCoords.Y / MazeMatrix.Num() - 0.5f;

		float EHeight = 0.0f;
		float HeightOctavesSum = 0.0f;
		for (int32 i = 0; i < HeightOctaves.Num(); i++)
		{
			EHeight += HeightOctaves[i].X * FMath::GetMappedRangeValueUnclamped(FVector2d(-1.0f, 1.0f), FVector2d(0.0f, 1.0f),
			                                                                    PerlinNoise2D(FVector2D(
				                                                                    HeightOctaves[i].Y * nx, HeightOctaves[i].Y * ny)));
			HeightOctavesSum += HeightOctaves[i].X;
		}

		if (HeightRedistribution) RoomState.Value.Height = HeightRedistribution->GetFloatValue(EHeight / HeightOctavesSum);
		else RoomState.Value.Height = EHeight / HeightOctavesSum;
	}
	PerlinReset(PerlinNoiseStream);
	for (auto& RoomState : MazeStateSubsystem->RoomsStates)
	{
		float nx = RoomState.Value.GridCoords.X / MazeMatrix[0].Num() - 0.5f;
		float ny = RoomState.Value.GridCoords.Y / MazeMatrix.Num() - 0.5f;

		float EHeat = 0.0f;
		float HeatOctavesSum = 0.0f;
		for (int32 i = 0; i < HeatOctaves.Num(); i++)
		{
			EHeat += HeatOctaves[i].X * FMath::GetMappedRangeValueUnclamped(FVector2d(-1.0f, 1.0f), FVector2d(0.0f, 1.0f),
			                                                                PerlinNoise2D(FVector2D(
				                                                                HeatOctaves[i].Y * nx, HeatOctaves[i].Y * ny)));
			HeatOctavesSum += HeatOctaves[i].X;
		}
		if (HeatRedistribution) RoomState.Value.Heat = HeatRedistribution->GetFloatValue(EHeat / HeatOctavesSum);
		else RoomState.Value.Heat = EHeat / HeatOctavesSum;
	}
	PerlinReset(PerlinNoiseStream);
	for (auto& RoomState : MazeStateSubsystem->RoomsStates)
	{
		float nx = RoomState.Value.GridCoords.X / MazeMatrix[0].Num() - 0.5f;
		float ny = RoomState.Value.GridCoords.Y / MazeMatrix.Num() - 0.5f;

		float EMoisture = 0.0f;
		float MoistureOctavesSum = 0.0f;
		for (int32 i = 0; i < MoistureOctaves.Num(); i++)
		{
			EMoisture += MoistureOctaves[i].X * FMath::GetMappedRangeValueUnclamped(FVector2d(-1.0f, 1.0f), FVector2d(0.0f, 1.0f),
			                                                                        PerlinNoise2D(FVector2D(
				                                                                        MoistureOctaves[i].Y * nx, MoistureOctaves[i].Y * ny)));
			MoistureOctavesSum += MoistureOctaves[i].X;
		}

		if (MoistureRedistribution) RoomState.Value.Moisture = MoistureRedistribution->GetFloatValue(EMoisture / MoistureOctavesSum);
		else RoomState.Value.Moisture = EMoisture / MoistureOctavesSum;
	}

	TArray<FColor> HeightTextureColor;
	HeightTextureColor.SetNum(10000);
	TArray<FColor> HeatTextureColor;
	HeatTextureColor.SetNum(10000);
	TArray<FColor> MoistureTextureColor;
	MoistureTextureColor.SetNum(10000);
	for (const auto& RoomState : MazeStateSubsystem->RoomsStates)
	{
		HeightTextureColor[RoomState.Value.GridCoords.X / 250.0f + RoomState.Value.GridCoords.Y / 250.0f * MazeMatrix[0].Num()] = FColor(
			RoomState.Value.Height * 255, RoomState.Value.Height * 255, RoomState.Value.Height * 255);
		HeatTextureColor[RoomState.Value.GridCoords.X / 250.0f + RoomState.Value.GridCoords.Y / 250.0f * MazeMatrix[0].Num()] = FColor(
			RoomState.Value.Heat * 255, RoomState.Value.Heat * 255, RoomState.Value.Heat * 255);
		MoistureTextureColor[RoomState.Value.GridCoords.X / 250.0f + RoomState.Value.GridCoords.Y / 250.0f * MazeMatrix[0].Num()] = FColor(
			RoomState.Value.Moisture * 255, RoomState.Value.Moisture * 255, RoomState.Value.Moisture * 255);
	}

	Returnable.Add(FImageUtils::CreateTexture2D(100, 100, HeightTextureColor, this, "TextureHgh", RF_WillBeLoaded, FCreateTexture2DParameters()));
	Returnable.Add(FImageUtils::CreateTexture2D(100, 100, HeatTextureColor, this, "TextureHea", RF_WillBeLoaded, FCreateTexture2DParameters()));
	Returnable.Add(FImageUtils::CreateTexture2D(100, 100, MoistureTextureColor, this, "TextureM", RF_WillBeLoaded, FCreateTexture2DParameters()));


	//назначить биомы
	for (auto& RoomState : MazeStateSubsystem->RoomsStates)
	{
		TArray<TPair<FName, float>> BiomesDistances;
		for (UMGBiomeData* Biome : Biomes)
		{
			BiomesDistances.Add({Biome->BiomeName, 0.0f});
			for (const auto& Weight : Biome->Weights)
			{
				switch (Weight.Key)
				{
				case Height:
					BiomesDistances.Last().Value += FMath::Abs(Weight.Value - RoomState.Value.Height);
					break;
				case Heat:
					BiomesDistances.Last().Value += FMath::Abs(Weight.Value - RoomState.Value.Heat);
					break;
				case Moisture:
					BiomesDistances.Last().Value += FMath::Abs(Weight.Value - RoomState.Value.Moisture);
					break;
				default:
					break;
				}
			}
		}
		float Min = INFINITY;
		int32 MinIndex = INFINITY;
		for (int32 i = 0; i < BiomesDistances.Num(); i++)
			if (BiomesDistances[i].Value < Min)
			{
				Min = BiomesDistances[i].Value;
				MinIndex = i;
			}
		if (MinIndex == INFINITY)
		{
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red,
				                                 "It is impossible to find the best biome for the room! Generation cancelled");
			UE_LOG(LogTemp, Error, TEXT("It is impossible to find the best biome for the room! Generation cancelled"));
			return Returnable;
		}
		RoomState.Value.BiomeName = BiomesDistances[MinIndex].Key;
	}
	TArray<FColor> BiomesTextureColor;
	BiomesTextureColor.SetNum(10000);
	for (const auto& RoomState : MazeStateSubsystem->RoomsStates)
		BiomesTextureColor[RoomState.Value.GridCoords.X / 250.0f + RoomState.Value.GridCoords.Y / 250.0f * MazeMatrix[0].Num()] =
			UMGBiomeData::GetBiomeByName(RoomState.Value.BiomeName, Biomes)->DebugColor;
	Returnable.Add(FImageUtils::CreateTexture2D(100, 100, BiomesTextureColor, this, "TextureBio", RF_WillBeLoaded, FCreateTexture2DParameters()));

	//создать и заполнить рандомно перемешанные массивы индексов комнат по биомам
	TArray<TPair<FName, TMap<int32, FRoomState>>> RoomsIDsByBiomesTemp;
	TArray<TPair<FName, TArray<TPair<int32, FRoomState>>>> RoomsIDsByBiomes;
	for (const auto Biome : Biomes)
	{
		TMap<int32, FRoomState> EmptyMap;
		RoomsIDsByBiomesTemp.Add({Biome->BiomeName, EmptyMap});
		TArray<TPair<int32, FRoomState>> EmptyArray;
		RoomsIDsByBiomes.Add({Biome->BiomeName, EmptyArray});
	}
	FRandomStream ShuffleStream(Seed);
	for (int32 i = 0; i < RoomsIDsByBiomesTemp.Num(); i++)
	{
		for (auto RoomsState : MazeStateSubsystem->RoomsStates)
			if (RoomsState.Value.BiomeName.IsEqual(RoomsIDsByBiomesTemp[i].Key))
				RoomsIDsByBiomesTemp[i].Value.Emplace(RoomsState.Key, RoomsState.Value);

		RoomsIDsByBiomesTemp[i].Value.KeySort([](int32 A, int32 B) { return A > B; });
		TArray<int32> Keys;
		TArray<FRoomState> Values;
		RoomsIDsByBiomesTemp[i].Value.GenerateKeyArray(Keys);
		RoomsIDsByBiomesTemp[i].Value.GenerateValueArray(Values);
		for (int32 j = 0; j < Keys.Num(); j++)
			RoomsIDsByBiomes[i].Value.Add({Keys[j], Values[j]});
		ShuffleArray(RoomsIDsByBiomes[i].Value, ShuffleStream);
	}

	//заспавнить плоские комнаты (заполнить инфу в MGMazeStateSubsystem о плоских комнатах)
	for (auto Biome : Biomes)
		for (auto Room : Biome->Rooms)
			if (Room.bIsFlat && Room.SpawnType == NumberOfRooms)
			{
				for (int i = 0; i < Room.SpawnRate; i++)
				{
					//попробовать выбрать комнату (изза плоскости)
					bool RoomChosenSuccessfully = false;
					while (!RoomChosenSuccessfully)
					{
						
					}
				}
			}
	for (auto Biome : Biomes)
		for (auto Room : Biome->Rooms)
			if (Room.bIsFlat && Room.SpawnType == SpawnRate)
			{
			}
	for (auto Biome : Biomes)
		for (auto Room : Biome->Rooms)
			if (Room.bIsFlat && Room.SpawnType == SpawnWeight)
			{
			}

	//заспавнить остальные (не плоские) комнаты (заполнить инфу в MGMazeStateSubsystem о не плоских комнатах)
	for (auto Biome : Biomes)
		for (auto Room : Biome->Rooms)
			if (!Room.bIsFlat && Room.SpawnType == NumberOfRooms)
			{
			}
	for (auto Biome : Biomes)
		for (auto Room : Biome->Rooms)
			if (!Room.bIsFlat && Room.SpawnType == SpawnRate)
			{
			}
	for (auto Biome : Biomes)
		for (auto Room : Biome->Rooms)
			if (!Room.bIsFlat && Room.SpawnType == SpawnWeight)
			{
			}

	//назначить комнаты (чанки)

	//рассчитать углы для комнат (и тупики)

	return Returnable;
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
	//const FMazeItem* ExitCell = nullptr;
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
				if (IndexY == 0 || IndexY == MatrixAddress.Num() - 1 || IndexX == 0 || IndexX == MatrixAddress[IndexY].
					Num() - 1)
					MatrixAddress[IndexY][IndexX].bIsUnbreakable = true;
			}
			else if ((IndexY == 0 || IndexY == MatrixAddress.Num() - 1) && !MatrixAddress[IndexY][IndexX].
				bIsUnbreakable)
			{
				if (EdgesIter != Exit)
				{
					MatrixAddress[IndexY][IndexX].MazeItemState = HorizontalEdge;
					MatrixAddress[IndexY][IndexX].bIsUnbreakable = true;
				}
				else ExitCell = FMazeItem(MatrixAddress[IndexY][IndexX].IndexY, MatrixAddress[IndexY][IndexX].IndexX);
				EdgesIter++;
			}
			else if ((IndexX == 0 || IndexX == MatrixAddress[IndexY].Num() - 1) && !MatrixAddress[IndexY][IndexX].
				bIsUnbreakable)
			{
				if (EdgesIter != Exit)
				{
					MatrixAddress[IndexY][IndexX].MazeItemState = VerticalEdge;
					MatrixAddress[IndexY][IndexX].bIsUnbreakable = true;
				}
				else ExitCell = FMazeItem(MatrixAddress[IndexY][IndexX].IndexY, MatrixAddress[IndexY][IndexX].IndexX);
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
		UE_LOG(LogTemp, Warning,
		       TEXT(
			       "The MazeMatrix is not generated. Either input values are 0 or BeginPlay() called too early for seed %d"
		       ), 12345);
		return;
	}
	if (ExitCell.IndexX == 0 && ExitCell.IndexY == 0)
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Error, TEXT("The exit is not generated for some reason for seed %d"), 12345);
		bIsMistakeHappenedInAsync = true;
		return;
	}
	TArray<FMazeItem*> DesiredCells;
	DesiredCells.Emplace(GetNearCellsByIndexes(ExitCell.IndexY, ExitCell.IndexX)[0]);
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

		FMazeItem* CurrentCell = DesiredCells[Stream.RandRange(0, DesiredCells.Num() - 1)];
		CurrentCell->bIsVisited = true;

		TArray<FMazeItem*> IterationNearCells;
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
			PlaceWallBetweenCellsByIndexes(IterationNearRemainingCell->IndexY, IterationNearRemainingCell->IndexX,
			                               CurrentCell->IndexY, CurrentCell->IndexX);
		}

		DesiredCells.Remove(CurrentCell);

		if (DesiredCells.IsEmpty())
		{
			FMazeItem* NotVisitedCell = nullptr;
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
				TArray<FMazeItem*> NearVisitedCellsForNotVisitedCell = GetNearCellsByIndexes(
					NotVisitedCell->IndexY, NotVisitedCell->IndexX, true);
				for (int32 i = NearVisitedCellsForNotVisitedCell.Num() - 1; i >= 0; i--)
					if (!NearVisitedCellsForNotVisitedCell[i]->bIsVisited)
						NearVisitedCellsForNotVisitedCell.
							RemoveAt(i);

				if (NearVisitedCellsForNotVisitedCell.Num() > 0)
				{
					const int32 RandomizedNearVisitedCellsForNotVisitedCellIndex = Stream.RandRange(
						0, NearVisitedCellsForNotVisitedCell.Num() - 1);

					BreakWallBetweenCellsByIndexes(NotVisitedCell->IndexY, NotVisitedCell->IndexX,
					                               NearVisitedCellsForNotVisitedCell[
						                               RandomizedNearVisitedCellsForNotVisitedCellIndex]->IndexY,
					                               NearVisitedCellsForNotVisitedCell[
						                               RandomizedNearVisitedCellsForNotVisitedCellIndex]->IndexX);
				}
				DesiredCells.Emplace(NotVisitedCell);
			}
		}
		UGameplayStatics::GetAccurateRealTime(SecondsAtBeginning, PartialSecondsAtBeginning);
	}
	while (!DesiredCells.IsEmpty());

	EndDelegate.ExecuteIfBound(bIsMistakeHappenedInAsync);
}

TArray<FMazeItem*> FGenerateMazeMatrixAsyncTask::GetNearCellsByIndexes(int32 IndexY, int32 IndexX, bool bIgnoreWalls)
{
	TArray<FMazeItem*> Returning;

	if (IndexY > MatrixAddress.Num() - 1 || IndexX > MatrixAddress[0].Num() || IndexY < 0 || IndexX < 0)
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Error, TEXT("Index out of bounds in function GetNearCellsByIndexes(%d, %d) with seed %d"),
		       IndexY, IndexX, 12345);
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
	else if (MatrixAddress[IndexY][IndexX].MazeItemState == HorizontalEdge || MatrixAddress[IndexY][IndexX].
		MazeItemState == VerticalEdge || MatrixAddress[IndexY][IndexX].
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
		UE_LOG(LogTemp, Error,
		       TEXT("Trying to get near cells not for Edge, Cell or None in indexes Y:%d X:%d with seed %d"), IndexY,
		       IndexX, 12345);
		bIsMistakeHappenedInAsync = true;
	}
	return Returning;
}

void FGenerateMazeMatrixAsyncTask::PlaceWallBetweenCellsByIndexes(int32 IndexY1, int32 IndexX1, int32 IndexY2,
                                                                  int32 IndexX2)
{
	if (MatrixAddress[IndexY1][IndexX1].MazeItemState != Cell || MatrixAddress[IndexY2][IndexX2].MazeItemState != Cell)
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "Trying to place walls not between cells in PlaceWallBetweenCellsByIndexes(%d, %d, %d, %d) with seed %d"
		       ), IndexY1, IndexX1, IndexY2, IndexX2, 12345);
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
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "The wall was not placed between the cells in PlaceWallBetweenCellsByIndexes(%d, %d, %d, %d) with seed %d"
		       ), IndexY1, IndexX1, IndexY2, IndexX2, 12345);
		bIsMistakeHappenedInAsync = true;
	}
}

void FGenerateMazeMatrixAsyncTask::BreakWallBetweenCellsByIndexes(int32 IndexY1, int32 IndexX1, int32 IndexY2,
                                                                  int32 IndexX2)
{
	if (MatrixAddress[IndexY1][IndexX1].MazeItemState != Cell || MatrixAddress[IndexY2][IndexX2].MazeItemState != Cell)
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "Trying to break walls not between cells in BreakWallBetweenCellsByIndexes(%d, %d, %d, %d) with seed %d"
		       ), IndexY1, IndexX1, IndexY2, IndexX2, 12345);
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
		UE_LOG(LogTemp, Error,
		       TEXT(
			       "The wall was not broken between the cells in BreakWallBetweenCellsByIndexes(%d, %d, %d, %d) with seed %d"
		       ), IndexY1, IndexX1, IndexY2, IndexX2, 12345);
		bIsMistakeHappenedInAsync = true;
	}
}

namespace FMGMathPerlinHelpers
{
	// random permutation of 256 numbers, repeated 2x
	static int32 Permutation[512] = {
		63, 9, 212, 205, 31, 128, 72, 59, 137, 203, 195, 170, 181, 115, 165, 40, 116, 139, 175, 225, 132, 99, 222, 2, 41, 15, 197, 93, 169, 90, 228,
		43, 221, 38, 206, 204, 73, 17, 97, 10, 96, 47, 32, 138, 136, 30, 219, 78, 224, 13, 193, 88, 134, 211, 7, 112, 176, 19, 106, 83, 75, 217, 85,
		0, 98, 140, 229, 80, 118, 151, 117, 251, 103, 242, 81, 238, 172, 82, 110, 4, 227, 77, 243, 46, 12, 189, 34, 188, 200, 161, 68, 76, 171, 194,
		57, 48, 247, 233, 51, 105, 5, 23, 42, 50, 216, 45, 239, 148, 249, 84, 70, 125, 108, 241, 62, 66, 64, 240, 173, 185, 250, 49, 6, 37, 26, 21,
		244, 60, 223, 255, 16, 145, 27, 109, 58, 102, 142, 253, 120, 149, 160, 124, 156, 79, 186, 135, 127, 14, 121, 22, 65, 54, 153, 91, 213, 174,
		24, 252, 131, 192, 190, 202, 208, 35, 94, 231, 56, 95, 183, 163, 111, 147, 25, 67, 36, 92, 236, 71, 166, 1, 187, 100, 130, 143, 237, 178, 158,
		104, 184, 159, 177, 52, 214, 230, 119, 87, 114, 201, 179, 198, 3, 248, 182, 39, 11, 152, 196, 113, 20, 232, 69, 141, 207, 234, 53, 86, 180,
		226, 74, 150, 218, 29, 133, 8, 44, 123, 28, 146, 89, 101, 154, 220, 126, 155, 122, 210, 168, 254, 162, 129, 33, 18, 209, 61, 191, 199, 157,
		245, 55, 164, 167, 215, 246, 144, 107, 235,

		63, 9, 212, 205, 31, 128, 72, 59, 137, 203, 195, 170, 181, 115, 165, 40, 116, 139, 175, 225, 132, 99, 222, 2, 41, 15, 197, 93, 169, 90, 228,
		43, 221, 38, 206, 204, 73, 17, 97, 10, 96, 47, 32, 138, 136, 30, 219, 78, 224, 13, 193, 88, 134, 211, 7, 112, 176, 19, 106, 83, 75, 217, 85,
		0, 98, 140, 229, 80, 118, 151, 117, 251, 103, 242, 81, 238, 172, 82, 110, 4, 227, 77, 243, 46, 12, 189, 34, 188, 200, 161, 68, 76, 171, 194,
		57, 48, 247, 233, 51, 105, 5, 23, 42, 50, 216, 45, 239, 148, 249, 84, 70, 125, 108, 241, 62, 66, 64, 240, 173, 185, 250, 49, 6, 37, 26, 21,
		244, 60, 223, 255, 16, 145, 27, 109, 58, 102, 142, 253, 120, 149, 160, 124, 156, 79, 186, 135, 127, 14, 121, 22, 65, 54, 153, 91, 213, 174,
		24, 252, 131, 192, 190, 202, 208, 35, 94, 231, 56, 95, 183, 163, 111, 147, 25, 67, 36, 92, 236, 71, 166, 1, 187, 100, 130, 143, 237, 178, 158,
		104, 184, 159, 177, 52, 214, 230, 119, 87, 114, 201, 179, 198, 3, 248, 182, 39, 11, 152, 196, 113, 20, 232, 69, 141, 207, 234, 53, 86, 180,
		226, 74, 150, 218, 29, 133, 8, 44, 123, 28, 146, 89, 101, 154, 220, 126, 155, 122, 210, 168, 254, 162, 129, 33, 18, 209, 61, 191, 199, 157,
		245, 55, 164, 167, 215, 246, 144, 107, 235
	};

	// Gradient functions for 1D, 2D and 3D Perlin noise

	FORCEINLINE float Grad1(int32 Hash, float X)
	{
		// Slicing Perlin's 3D improved noise would give us only scales of -1, 0 and 1; this looks pretty bad so let's use a different sampling
		static const float Grad1Scales[16] = {
			-8 / 8, -7 / 8., -6 / 8., -5 / 8., -4 / 8., -3 / 8., -2 / 8., -1 / 8., 1 / 8., 2 / 8., 3 / 8., 4 / 8., 5 / 8., 6 / 8., 7 / 8., 8 / 8
		};
		return Grad1Scales[Hash & 15] * X;
	}

	// Note: If you change the Grad2 or Grad3 functions, check that you don't change the range of the resulting noise as well; it should be (within floating point error) in the range of (-1, 1)
	FORCEINLINE float Grad2(int32 Hash, float X, float Y)
	{
		// corners and major axes (similar to the z=0 projection of the cube-edge-midpoint sampling from improved Perlin noise)
		switch (Hash & 7)
		{
		case 0: return X;
		case 1: return X + Y;
		case 2: return Y;
		case 3: return -X + Y;
		case 4: return -X;
		case 5: return -X - Y;
		case 6: return -Y;
		case 7: return X - Y;
		// can't happen
		default: return 0;
		}
	}

	FORCEINLINE float Grad3(int32 Hash, float X, float Y, float Z)
	{
		switch (Hash & 15)
		{
		// 12 cube midpoints
		case 0: return X + Z;
		case 1: return X + Y;
		case 2: return Y + Z;
		case 3: return -X + Y;
		case 4: return -X + Z;
		case 5: return -X - Y;
		case 6: return -Y + Z;
		case 7: return X - Y;
		case 8: return X - Z;
		case 9: return Y - Z;
		case 10: return -X - Z;
		case 11: return -Y - Z;
		// 4 vertices of regular tetrahedron
		case 12: return X + Y;
		case 13: return -X + Y;
		case 14: return -Y + Z;
		case 15: return -Y - Z;
		// can't happen
		default: return 0;
		}
	}

	// Curve w/ second derivative vanishing at 0 and 1, from Perlin's improved noise paper
	FORCEINLINE float SmoothCurve(float X)
	{
		return X * X * X * (X * (X * 6.0f - 15.0f) + 10.0f);
	}
};

void AMGMazeGenerator::PerlinReset(FRandomStream& Stream)
{
	using namespace FMGMathPerlinHelpers;
	TArray<int32> Numbers;
	Numbers.SetNumUninitialized(256);
	for (int32 i = 0; i < 256; ++i)
	{
		Numbers[i] = i;
	}
	for (int32 i = 0; i < 256; ++i)
	{
		int32 Index = Stream.RandRange(0, 255 - i);
		Permutation[i] = Numbers[Index];
		Permutation[i + 256] = Numbers[Index];
		Numbers.Remove(Index);
	}
}

float AMGMazeGenerator::PerlinNoise2D(const FVector2D& Location)
{
	using namespace FMGMathPerlinHelpers;

	float Xfl = FMath::FloorToFloat((float)Location.X); // LWC_TODO: Precision loss
	float Yfl = FMath::FloorToFloat((float)Location.Y);
	int32 Xi = (int32)(Xfl) & 255;
	int32 Yi = (int32)(Yfl) & 255;
	float X = (float)Location.X - Xfl;
	float Y = (float)Location.Y - Yfl;
	float Xm1 = X - 1.0f;
	float Ym1 = Y - 1.0f;

	const int32* P = Permutation;
	int32 AA = P[Xi] + Yi;
	int32 AB = AA + 1;
	int32 BA = P[Xi + 1] + Yi;
	int32 BB = BA + 1;

	float U = SmoothCurve(X);
	float V = SmoothCurve(Y);

	// Note: Due to the choice of Grad2, this will be in the (-1,1) range with no additional scaling
	return FMath::Lerp(
		FMath::Lerp(Grad2(P[AA], X, Y), Grad2(P[BA], Xm1, Y), U),
		FMath::Lerp(Grad2(P[AB], X, Ym1), Grad2(P[BB], Xm1, Ym1), U),
		V);
}
