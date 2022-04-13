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

void AMGMazeGenerator::LaunchAsyncMazeGeneration(int32 Seed, int32 XSize, int32 YSize)
{
	(new FAutoDeleteAsyncTask<FGenerateMazeMatrixAsyncTask>(MatrixGenerationFinishedDelegate, DrawGenerationProgressUIDelegate, MazeMatrix, Seed, YSize, XSize))->
		StartBackgroundTask();
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

void AMGMazeGenerator::BeginPlay()
{
	Super::BeginPlay();

	if (MazeGenerationProgressWidgetRef)
	{
		MazeGenerationProgressWidgetRef->AddToViewport(1);
	}

	LaunchAsyncMazeGeneration(MGSeed, MGXSize, MGYSize);

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
		GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Green, FString::Printf(TEXT("The maze matrix was successfully async generated")));

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
	DrawGenerationProgressUIDelegate.ExecuteIfBound(Text);

	//MazeGenerationProgressWidgetRef->RemoveFromViewport();
	//SpawnWithInstances();
}

void AMGMazeGenerator::DrawGenerationProgressUI(FString StringToDisplay)
{
	FText Text = FText::FromString(StringToDisplay);

	FFunctionGraphTask::CreateAndDispatchWhenReady([this,Text]()
	{
		MazeGenerationProgressWidgetRef->TextBlock->SetText(Text);
	}, TStatId(), nullptr, ENamedThreads::GameThread);
}

void FGenerateMazeMatrixAsyncTask::DoWork()
{
	const FRandomStream Stream(Seed/*FMath::Rand()*/);

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

		if (PastTime >= 0.00000001f/*0.000001f*//*0.00000000001f*/)
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
