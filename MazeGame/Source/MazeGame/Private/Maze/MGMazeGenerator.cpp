// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/MGMazeGenerator.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Maze/MGInstancedMeshActor.h"

AMGMazeGenerator::AMGMazeGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMGMazeGenerator::GenerateMazeWithSeed(int32 Seed, int32 YSize, int32 XSize)
{
	FRandomStream Stream(Seed/*FMath::Rand()*/);

	MazeMatrix.Empty();
	MazeMatrix.SetNum(YSize * 2 + 1);
	for (auto& MazeItemY : MazeMatrix)
		MazeItemY.SetNum(XSize * 2 + 1);

	MazeMatrix[0][0].MazeItemState = Knot;
	MazeMatrix[0][0].bIsUnbreakable = true;
	MazeMatrix[0][MazeMatrix[0].Num() - 1].MazeItemState = Knot;
	MazeMatrix[0][MazeMatrix[0].Num() - 1].bIsUnbreakable = true;
	MazeMatrix[MazeMatrix.Num() - 1][0].MazeItemState = Knot;
	MazeMatrix[MazeMatrix.Num() - 1][0].bIsUnbreakable = true;
	MazeMatrix[MazeMatrix.Num() - 1][MazeMatrix[0].Num() - 1].MazeItemState = Knot;
	MazeMatrix[MazeMatrix.Num() - 1][MazeMatrix[0].Num() - 1].bIsUnbreakable = true;

	const int32 Exit = Stream.RandRange(0, 2 * (YSize + XSize) - 1);
	MazeItem* ExitCell = nullptr;
	int32 EdgesIter = 0;
	for (int32 IndexY = 0; IndexY != MazeMatrix.Num(); ++IndexY)
	{
		for (int32 IndexX = 0; IndexX != MazeMatrix[IndexY].Num(); ++IndexX)
		{
			MazeMatrix[IndexY][IndexX].IndexX = IndexX;
			MazeMatrix[IndexY][IndexX].IndexY = IndexY;
			if (IndexX % 2 == 0 && IndexY % 2 == 0)
			{
				MazeMatrix[IndexY][IndexX].MazeItemState = Knot;
				if (IndexY == 0 || IndexY == MazeMatrix.Num() - 1 || IndexX == 0 || IndexX == MazeMatrix[IndexY].Num() - 1)
					MazeMatrix[IndexY][IndexX].bIsUnbreakable = true;
			}
			else if ((IndexY == 0 || IndexY == MazeMatrix.Num() - 1) && !MazeMatrix[IndexY][IndexX].bIsUnbreakable)
			{
				if (EdgesIter != Exit)
				{
					MazeMatrix[IndexY][IndexX].MazeItemState = HorizontalEdge;
					MazeMatrix[IndexY][IndexX].bIsUnbreakable = true;
				}
				else ExitCell = &MazeMatrix[IndexY][IndexX];
				EdgesIter++;
			}
			else if ((IndexX == 0 || IndexX == MazeMatrix[IndexY].Num() - 1) && !MazeMatrix[IndexY][IndexX].bIsUnbreakable)
			{
				if (EdgesIter != Exit)
				{
					MazeMatrix[IndexY][IndexX].MazeItemState = VerticalEdge;
					MazeMatrix[IndexY][IndexX].bIsUnbreakable = true;
				}
				else ExitCell = &MazeMatrix[IndexY][IndexX];
				EdgesIter++;
			}
			else if (IndexX % 2 == 1 && IndexY % 2 == 1)
			{
				MazeMatrix[IndexY][IndexX].MazeItemState = Cell;
			}
		}
	}

	/*TArray<TArray<MazeItem*>> HelperShuffledArray;
	HelperShuffledArray.SetNum(YSize * 2 + 1);
	for (auto& MazeItemY : HelperShuffledArray)
		MazeItemY.SetNum(XSize * 2 + 1);
	for (int32 IndexY = 0; IndexY != HelperShuffledArray.Num(); ++IndexY)
		for (int32 IndexX = 0; IndexX != HelperShuffledArray[IndexY].Num(); ++IndexX)
			HelperShuffledArray[IndexY][IndexX] = &MazeMatrix[IndexY][IndexX];*/

	/*for (auto& HelperShuffledArrayY : HelperShuffledArray)
		ShuffleArrayByStream(HelperShuffledArrayY, Stream);
	ShuffleArrayByStream(HelperShuffledArray, Stream);*/

	if (MazeMatrix.Num() < 2)
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Warning, TEXT("The MazeMatrix is not generated. Either input values are 0 or BeginPlay() called too early for seed %d"), 12345);
		return;
	}
	if (ExitCell == nullptr)
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Error, TEXT("The exit is not generated for some reason for seed %d"), 12345);
		bIsMistakeHappened = true;
		return;
	}
	TArray<MazeItem*> DesiredCells;
	DesiredCells.Emplace(GetNearCellsByIndexes(ExitCell->IndexY, ExitCell->IndexX)[0]);
	DesiredCells[0]->Distance = 1;

	do
	{
		MazeItem* CurrentCell = DesiredCells[Stream.RandRange(0, DesiredCells.Num() - 1)];
		CurrentCell->bIsVisited = true;

		TArray<MazeItem*> IterationNearCells;
		IterationNearCells.Append(GetNearCellsByIndexes(CurrentCell->IndexY, CurrentCell->IndexX));
		for (int32 i = IterationNearCells.Num() - 1; i >= 0; i--)
			if (IterationNearCells[i]->bIsVisited) IterationNearCells.RemoveAt(i);

		const int32 R = Stream.RandRange(0, IterationNearCells.Num());
		/*if (IterationNearCells.Num() == 0||IterationNearCells.Num() == 1) R = 0;
		else R = Stream.RandRange(1, IterationNearCells.Num());*/
		for (int32 i = R; i > 1/*we always place at least 1 wall, if we want to leave empty knots, change this to 0*/; i--)
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

		/*//Older
		if (DesiredCells.IsEmpty())
		{
			bool bBreak = false;
			for (auto& MazeItemY : MazeMatrix) //TODO choose random element (not first)
			{
				for (auto& MazeItemX : MazeItemY)
				{
					if (MazeItemX.bIsVisited != true && MazeItemX.MazeItemState == Cell)
					{
						for (const auto& NearCell : GetNearCellsByIndexes(MazeItemX.IndexY, MazeItemX.IndexX, true))
						{
							if (NearCell->bIsVisited == true)
							{
								DesiredCells.Emplace(&MazeItemX); //TODO do not break the walls, if they leave empty knot
								BreakWallBetweenCellsByIndexes(DesiredCells[0]->IndexY, DesiredCells[0]->IndexX, NearCell->IndexY, NearCell->IndexX);
								bBreak = true;
								break;
							}
						}
					}
					if (bBreak) break;
				}
				if (bBreak) break;
			}
		}*/

		if (DesiredCells.IsEmpty())
		{
			/*MazeItem* NotVisitedCell = nullptr;
			bool bIsBreak = false;
			ShuffleArrayByStream(HelperShuffledArray, Stream);
			for (auto& HelperShuffledArrayY : HelperShuffledArray)
			{
				for (auto& HelperShuffledArrayX : HelperShuffledArrayY)
				{
					if (HelperShuffledArrayX->bIsVisited != true && HelperShuffledArrayX->MazeItemState == Cell)
						for (const auto& NearCell : GetNearCellsByIndexes(HelperShuffledArrayX->IndexY, HelperShuffledArrayX->IndexX, true))
							if (NearCell->bIsVisited == true)
							{
								NotVisitedCell = HelperShuffledArrayX;
								//ShuffleArrayByStream(HelperShuffledArrayY, Stream);
								bIsBreak = true;
								break;
							}
					if (bIsBreak) break;
				}
				if (bIsBreak) break;
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
			}*/

			MazeItem* NotVisitedCell = nullptr;
			int32 MaxDistance = -1;
			for (auto& MazeItemY : MazeMatrix)
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

			/*MazeItem* NotVisitedCell = nullptr;
			bool bIsBreak = false;
			for (auto& MazeItemY : MazeMatrix)
			{
				for (auto& MazeItemX : MazeItemY)
				{
					if (MazeItemX.bIsVisited != true && MazeItemX.MazeItemState == Cell)
						for (const auto& NearCell : GetNearCellsByIndexes(MazeItemX.IndexY, MazeItemX.IndexX, true))
							if (NearCell->bIsVisited == true)
							{
								NotVisitedCell = &MazeItemX;
								bIsBreak = true;
								break;
							}
					if (bIsBreak) break;
				}
				if (bIsBreak) break;
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
			}*/

			/*TArray<MazeItem*> NotVisitedCells;
			for (auto& MazeItemY : MazeMatrix)
				for (auto& MazeItemX : MazeItemY)
					if (MazeItemX.bIsVisited != true && MazeItemX.MazeItemState == Cell)
						for (const auto& NearCell : GetNearCellsByIndexes(MazeItemX.IndexY, MazeItemX.IndexX, true))
							if (NearCell->bIsVisited == true)
							{
								NotVisitedCells.Emplace(&MazeItemX);
								break;
							}

			if (NotVisitedCells.Num() != 0)
			{
				const int32 RandomizedCellIndex = Stream.RandRange(0, NotVisitedCells.Num() - 1);
				TArray<MazeItem*> NearVisitedCellsForRandomizedCell = GetNearCellsByIndexes(NotVisitedCells[RandomizedCellIndex]->IndexY,
				                                                                            NotVisitedCells[RandomizedCellIndex]->IndexX, true);

				for (int32 i = NearVisitedCellsForRandomizedCell.Num() - 1; i >= 0; i--)
					if (!NearVisitedCellsForRandomizedCell[i]->bIsVisited) NearVisitedCellsForRandomizedCell.RemoveAt(i);

				if (NearVisitedCellsForRandomizedCell.Num() > 0)
				{
					const int32 RandomizedNearVisitedCellsForRandomizedCellIndex = Stream.RandRange(0, NearVisitedCellsForRandomizedCell.Num() - 1);

					BreakWallBetweenCellsByIndexes(NotVisitedCells[RandomizedCellIndex]->IndexY, NotVisitedCells[RandomizedCellIndex]->IndexX,
					                               NearVisitedCellsForRandomizedCell[RandomizedNearVisitedCellsForRandomizedCellIndex]->IndexY,
					                               NearVisitedCellsForRandomizedCell[RandomizedNearVisitedCellsForRandomizedCellIndex]->IndexX);
				}

				/*if (NearVisitedCellsForRandomizedCell.Num() > 1)
				{
					if (NearVisitedCellsForRandomizedCell.Num() == 2)
						NearVisitedCellsForRandomizedCell.Swap(Stream.RandRange(0, NearVisitedCellsForRandomizedCell.Num() - 1),
						                                       Stream.RandRange(0, NearVisitedCellsForRandomizedCell.Num() - 1));
					else
					{
						NearVisitedCellsForRandomizedCell.Swap(Stream.RandRange(0, NearVisitedCellsForRandomizedCell.Num() - 1),
						                                       Stream.RandRange(0, NearVisitedCellsForRandomizedCell.Num() - 1));
						NearVisitedCellsForRandomizedCell.Swap(Stream.RandRange(0, NearVisitedCellsForRandomizedCell.Num() - 1),
						                                       Stream.RandRange(0, NearVisitedCellsForRandomizedCell.Num() - 1));
					}
				}

				for (const auto& NearCellForRandomizedCell : NearVisitedCellsForRandomizedCell)
					if (NearCellForRandomizedCell->bIsVisited == true)
					{
						BreakWallBetweenCellsByIndexes(NotVisitedCells[RandomizedCellIndex]->IndexY, NotVisitedCells[RandomizedCellIndex]->IndexX,
						                               NearCellForRandomizedCell->IndexY, NearCellForRandomizedCell->IndexX);
						break;
					}#1#
				DesiredCells.Emplace(NotVisitedCells[RandomizedCellIndex]);
			}*/
		}
	}
	while (!DesiredCells.IsEmpty());
}

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

void AMGMazeGenerator::BeginPlay()
{
	Super::BeginPlay();

	int32 SecondsAtBeginning;
	float PartialSecondsAtBeginning;
	int32 SecondsAfter;
	float PartialSecondsAfter;

	UGameplayStatics::GetAccurateRealTime(GetWorld(), SecondsAtBeginning, PartialSecondsAtBeginning);
	GenerateMazeWithSeed(MGSeed, MGYSize, MGXSize);
	UGameplayStatics::GetAccurateRealTime(GetWorld(), SecondsAfter, PartialSecondsAfter);
	float PastTime = SecondsAfter - SecondsAtBeginning + PartialSecondsAfter - PartialSecondsAtBeginning;
	GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, FString::Printf(TEXT("The maze was generated in %f seconds"), PastTime));

	PrintMazeMatrixToLog();
	if (bIsMistakeHappened && GEngine)
		GEngine->AddOnScreenDebugMessage(-1, INFINITY, FColor::Red, FString::Printf(TEXT("Errors occurred during generation! Check the log for more details.")));

	UGameplayStatics::GetAccurateRealTime(GetWorld(), SecondsAtBeginning, PartialSecondsAtBeginning);
	//SimpleSpawnGeneratedMaze();
	SpawnWithInstances();
	UGameplayStatics::GetAccurateRealTime(GetWorld(), SecondsAfter, PartialSecondsAfter);
	PastTime = SecondsAfter - SecondsAtBeginning + PartialSecondsAfter - PartialSecondsAtBeginning;
	GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, FString::Printf(TEXT("The maze was spawned in %f seconds"), PastTime));
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

TArray<MazeItem*> AMGMazeGenerator::GetNearCellsByIndexes(int32 IndexY, int32 IndexX, bool bIgnoreWalls)
{
	TArray<MazeItem*> Returning;

	if (IndexY > MazeMatrix.Num() - 1 || IndexX > MazeMatrix[0].Num() || IndexY < 0 || IndexX < 0)
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Error, TEXT("Index out of bounds in function GetNearCellsByIndexes(%d, %d) with seed %d"), IndexY, IndexX, 12345);
		bIsMistakeHappened = true;
		return Returning;
	}

	if (MazeMatrix[IndexY][IndexX].MazeItemState == Cell)
	{
		if (IndexY - 2 >= 0)
			if (MazeMatrix[IndexY - 2][IndexX].MazeItemState == Cell)
				if (bIgnoreWalls)
					Returning.Emplace(&MazeMatrix[IndexY - 2][IndexX]);
				else if (MazeMatrix[IndexY - 1][IndexX].MazeItemState != HorizontalEdge)
					Returning.Emplace(&MazeMatrix[IndexY - 2][IndexX]);
		if (IndexX - 2 >= 0)
			if (MazeMatrix[IndexY][IndexX - 2].MazeItemState == Cell)
				if (bIgnoreWalls)
					Returning.Emplace(&MazeMatrix[IndexY][IndexX - 2]);
				else if (MazeMatrix[IndexY][IndexX - 1].MazeItemState != VerticalEdge)
					Returning.Emplace(&MazeMatrix[IndexY][IndexX - 2]);
		if (IndexY + 2 <= MazeMatrix.Num() - 2)
			if (MazeMatrix[IndexY + 2][IndexX].MazeItemState == Cell)
				if (bIgnoreWalls)
					Returning.Emplace(&MazeMatrix[IndexY + 2][IndexX]);
				else if (MazeMatrix[IndexY + 1][IndexX].MazeItemState != HorizontalEdge)
					Returning.Emplace(&MazeMatrix[IndexY + 2][IndexX]);
		if (IndexX + 2 <= MazeMatrix[0].Num() - 2)
			if (MazeMatrix[IndexY][IndexX + 2].MazeItemState == Cell)
				if (bIgnoreWalls)
					Returning.Emplace(&MazeMatrix[IndexY][IndexX + 2]);
				else if (MazeMatrix[IndexY][IndexX + 1].MazeItemState != VerticalEdge)
					Returning.Emplace(&MazeMatrix[IndexY][IndexX + 2]);
	}
	else if (MazeMatrix[IndexY][IndexX].MazeItemState == HorizontalEdge || MazeMatrix[IndexY][IndexX].MazeItemState == VerticalEdge || MazeMatrix[IndexY][IndexX].MazeItemState
		== None)
	{
		if (IndexY - 1 >= 0)
			if (MazeMatrix[IndexY - 1][IndexX].MazeItemState == Cell)
				Returning.Emplace(&MazeMatrix[IndexY - 1][IndexX]);
		if (IndexX - 1 >= 0)
			if (MazeMatrix[IndexY][IndexX - 1].MazeItemState == Cell)
				Returning.Emplace(&MazeMatrix[IndexY][IndexX - 1]);
		if (IndexY + 1 <= MazeMatrix.Num() - 1)
			if (MazeMatrix[IndexY + 1][IndexX].MazeItemState == Cell)
				Returning.Emplace(&MazeMatrix[IndexY + 1][IndexX]);
		if (IndexX + 1 <= MazeMatrix[0].Num() - 1)
			if (MazeMatrix[IndexY][IndexX + 1].MazeItemState == Cell)
				Returning.Emplace(&MazeMatrix[IndexY][IndexX + 1]);
	}
	else
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Error, TEXT("Trying to get near cells not for Edge, Cell or None in indexes Y:%d X:%d with seed %d"), IndexY, IndexX, 12345);
		bIsMistakeHappened = true;
	}
	return Returning;
}

void AMGMazeGenerator::PlaceWallBetweenCellsByIndexes(int32 IndexY1, int32 IndexX1, int32 IndexY2, int32 IndexX2)
//TODO add check to not place wall between 2 visited cells
{
	if (MazeMatrix[IndexY1][IndexX1].MazeItemState != Cell || MazeMatrix[IndexY2][IndexX2].MazeItemState != Cell)
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Error, TEXT("Trying to place walls not between cells in PlaceWallBetweenCellsByIndexes(%d, %d, %d, %d) with seed %d"), IndexY1, IndexX1,
		       IndexY2, IndexX2, 12345);
		bIsMistakeHappened = true;
		return;
	}

	if (IndexY1 > IndexY2 && IndexX1 == IndexX2)
		MazeMatrix[IndexY1 - 1][IndexX1].MazeItemState = HorizontalEdge;
	else if (IndexY1 < IndexY2 && IndexX1 == IndexX2)
		MazeMatrix[IndexY1 + 1][IndexX1].MazeItemState = HorizontalEdge;
	else if (IndexX1 > IndexX2 && IndexY1 == IndexY2)
		MazeMatrix[IndexY1][IndexX1 - 1].MazeItemState = VerticalEdge;
	else if (IndexX1 < IndexX2 && IndexY1 == IndexY2)
		MazeMatrix[IndexY1][IndexX1 + 1].MazeItemState = VerticalEdge;
	else
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Error, TEXT("The wall was not placed between the cells in PlaceWallBetweenCellsByIndexes(%d, %d, %d, %d) with seed %d"), IndexY1,
		       IndexX1, IndexY2, IndexX2, 12345);
		bIsMistakeHappened = true;
	}
}

void AMGMazeGenerator::BreakWallBetweenCellsByIndexes(int32 IndexY1, int32 IndexX1, int32 IndexY2, int32 IndexX2)
{
	if (MazeMatrix[IndexY1][IndexX1].MazeItemState != Cell || MazeMatrix[IndexY2][IndexX2].MazeItemState != Cell)
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Error, TEXT("Trying to break walls not between cells in BreakWallBetweenCellsByIndexes(%d, %d, %d, %d) with seed %d"), IndexY1, IndexX1,
		       IndexY2, IndexX2, 12345);
		bIsMistakeHappened = true;
		return;
	}
	//TODO add check for not place empty knots
	if (IndexY1 > IndexY2 && IndexX1 == IndexX2)
		MazeMatrix[IndexY1 - 1][IndexX1].MazeItemState = None;
	else if (IndexY1 < IndexY2 && IndexX1 == IndexX2)
		MazeMatrix[IndexY1 + 1][IndexX1].MazeItemState = None;
	else if (IndexX1 > IndexX2 && IndexY1 == IndexY2)
		MazeMatrix[IndexY1][IndexX1 - 1].MazeItemState = None;
	else if (IndexX1 < IndexX2 && IndexY1 == IndexY2)
		MazeMatrix[IndexY1][IndexX1 + 1].MazeItemState = None;
	else
	{
		//TODO change seed to from gameinst
		UE_LOG(LogTemp, Error, TEXT("The wall was not broken between the cells in BreakWallBetweenCellsByIndexes(%d, %d, %d, %d) with seed %d"), IndexY1,
		       IndexX1, IndexY2, IndexX2, 12345);
		bIsMistakeHappened = true;
	}
}

void AMGMazeGenerator::ShuffleArrayByStream(TArray<MazeItem*>& ArrayToShuffle, FRandomStream& Stream)
{
	TArray<MazeItem*> Temp;
	do
	{
		int32 IterRand = Stream.RandRange(0, ArrayToShuffle.Num() - 1);
		Temp.Emplace(ArrayToShuffle[IterRand]);
		ArrayToShuffle.RemoveAt(IterRand);
	}
	while (!ArrayToShuffle.IsEmpty());

	ArrayToShuffle = Temp;
}

void AMGMazeGenerator::ShuffleArrayByStream(TArray<TArray<MazeItem*>>& ArrayOfTArrayToShuffle, FRandomStream& Stream)
{
	TArray<TArray<MazeItem*>> Temp;
	do
	{
		int32 IterRand = Stream.RandRange(0, ArrayOfTArrayToShuffle.Num() - 1);
		Temp.Emplace(ArrayOfTArrayToShuffle[IterRand]);
		ArrayOfTArrayToShuffle.RemoveAt(IterRand);
	}
	while (!ArrayOfTArrayToShuffle.IsEmpty());

	ArrayOfTArrayToShuffle = Temp;
}
