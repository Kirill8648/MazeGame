// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/Generation/MGMazeStateSubsystem.h"

#include "MGPlayerDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Maze/Generation/MGBiomeData.h"

int32 UMGMazeStateSubsystem::GetRoomObjectHash(AActor* Actor)
{
	int32 Hash = 0;
	if (const UMGPlayerDataSubsystem* PlayerDataSubsystem = UGameplayStatics::GetGameInstance(Actor->GetWorld())->GetSubsystem<UMGPlayerDataSubsystem>();
		PlayerDataSubsystem && Actor->Tags.Num() > 0)
	{
		const float GridSize = PlayerDataSubsystem->CurrentMazeCellSize/* / 2*/;

		FString StringHash;
		StringHash.Append(Actor->Tags.Last().ToString());
		StringHash.AppendInt(FMath::GridSnap(Actor->GetActorLocation().X, GridSize));
		StringHash.AppendInt(FMath::GridSnap(Actor->GetActorLocation().Y, GridSize));

		Hash = TextKeyUtil::HashString(StringHash);
	}
	else
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("Failed to get hash for actor %s"), *Actor->GetName()));
	}

	return Hash;
}

int32 UMGMazeStateSubsystem::GetRoomHash(UObject* WorldContextObject, const float X, const float Y)
{
	int32 Hash = 0;
	if (const UMGPlayerDataSubsystem* PlayerDataSubsystem = UGameplayStatics::GetGameInstance(WorldContextObject->GetWorld())->GetSubsystem<UMGPlayerDataSubsystem>())
	{
		const float GridSize = PlayerDataSubsystem->CurrentMazeCellSize/* / 2*/;
		FString StringHash;
		StringHash.AppendInt(FMath::GridSnap(X, GridSize));
		StringHash.AppendInt(FMath::GridSnap(Y, GridSize));

		Hash = TextKeyUtil::HashString(StringHash);
	}
	else
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, FString::Printf(TEXT("Failed to get hash for Level at X: %f; Y: %f"), X, Y));
	}

	return Hash;
}

bool UMGMazeStateSubsystem::TryToSpawn(FRoomInfo RoomToSpawn, double Angle, TArray<TPair<int32, FRoomState>>* BiomeRoomsToRemoveFrom, int32 RootIndexInBiomeRooms,
                                       bool bCanOverride)
{
	if (BiomeRoomsToRemoveFrom->IsEmpty())
	{
		return true;
	}
	bool bSuccessfullySpawned = false;
	int32 RoomPlaceHash = BiomeRoomsToRemoveFrom->Last(BiomeRoomsToRemoveFrom->Num() - 1 - RootIndexInBiomeRooms).Key;
	TArray<FVector2D> RotatedChunks_Temp = GetRotatedChunksArray(RoomToSpawn.Room->RoomChunks, Angle);
	FRoomState RoomPlaceState = RoomsStates.FindRef(RoomPlaceHash);
	if (RoomPlaceState.BiomeName.IsNone())
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red,
			                                 FString::Printf(TEXT("UMGMazeStateSubsystem::TryToSpawn called with invalid 'RoomPlaceHash' property: %d"), RoomPlaceHash));
		UE_LOG(LogTemp, Error, TEXT("UMGMazeStateSubsystem::TryToSpawn called with invalid 'RoomPlaceHash' property: %d"), RoomPlaceHash);
		return bSuccessfullySpawned;
	}

	bool bFlatCheckPassed = false;
	//чекнуть плоские комнаты
	if (RoomToSpawn.bIsFlat)
	{
		for (auto RotatedChunk_Temp : RotatedChunks_Temp)
		{
			TSet<FVector2D> ChunkCircle;
			ChunkCircle.Add({RotatedChunk_Temp.X - 1.0f, RotatedChunk_Temp.Y});
			ChunkCircle.Add({RotatedChunk_Temp.X + 1.0f, RotatedChunk_Temp.Y});
			ChunkCircle.Add({RotatedChunk_Temp.X, RotatedChunk_Temp.Y + 1.0f});
			ChunkCircle.Add({RotatedChunk_Temp.X, RotatedChunk_Temp.Y - 1.0f});
			ChunkCircle.Add({RotatedChunk_Temp.X - 1.0f, RotatedChunk_Temp.Y + 1.0f});
			ChunkCircle.Add({RotatedChunk_Temp.X + 1.0f, RotatedChunk_Temp.Y + 1.0f});
			ChunkCircle.Add({RotatedChunk_Temp.X + 1.0f, RotatedChunk_Temp.Y - 1.0f});
			ChunkCircle.Add({RotatedChunk_Temp.X - 1.0f, RotatedChunk_Temp.Y - 1.0f});
			for (auto Chunks_Temp : RotatedChunks_Temp)
				ChunkCircle.Remove(Chunks_Temp);

			for (auto ChunkCircleElement : ChunkCircle)
			{
				int32 Hash = GetRoomHash(GetWorld(), (RoomPlaceState.Coords.X / GridSize + ChunkCircleElement.X) * GridSize,
				                         (RoomPlaceState.Coords.Y / GridSize + ChunkCircleElement.Y) * GridSize);

				if (FRoomState* ChunkCircleElementState = RoomsStates.Find(Hash))
				{
					if (ChunkCircleElementState->bIsFlat)
						return bSuccessfullySpawned;
				}
				else
				{
					//TODO переделать чтобы искало только в близлежащей области
					for (auto& RoomState : RoomsStates)
						for (auto Chunk : RoomState.Value.Chunks)
							if (RoomPlaceState.Coords / GridSize + ChunkCircleElement == FVector2D(Chunk.ChunkLocalCoords.X, Chunk.ChunkLocalCoords.Y).
								GetRotated(RoomState.Value.Rotation.Yaw) + FVector2D(RoomState.Value.Coords / GridSize))
								if (RoomState.Value.bIsFlat)
									return bSuccessfullySpawned;
				}
			}
		}
		bFlatCheckPassed = true;
	}
	else bFlatCheckPassed = true;

	bool bShouldNotBeDeletedByChunksCheckPassed = false;
	//чекнуть незаменяемые комнаты
	for (auto RotatedChunk_Temp : RotatedChunks_Temp)
	{
		int32 Hash = GetRoomHash(GetWorld(), (RoomPlaceState.Coords.X / GridSize + RotatedChunk_Temp.X) * GridSize,
		                         (RoomPlaceState.Coords.Y / GridSize + RotatedChunk_Temp.Y) * GridSize);

		if (FRoomState* ChunkState = RoomsStates.Find(Hash))
		{
			if (ChunkState->bIsFlat || ChunkState->bShouldNotBeDeletedByChunks)
				return bSuccessfullySpawned;
		}
		else
		{
			//TODO переделать чтобы искало только в близлежащей области
			for (auto& RoomState : RoomsStates)
				for (auto Chunk : RoomState.Value.Chunks)
					if (RoomPlaceState.Coords / GridSize + RotatedChunk_Temp == FVector2D(Chunk.ChunkLocalCoords.X, Chunk.ChunkLocalCoords.Y).
						GetRotated(RoomState.Value.Rotation.Yaw) + FVector2D(RoomState.Value.Coords / GridSize))
						if (RoomState.Value.bIsFlat || RoomState.Value.bShouldNotBeDeletedByChunks)
							return bSuccessfullySpawned;
		}
	}
	bShouldNotBeDeletedByChunksCheckPassed = true;

	//TODO добавить чек на границы массива (лабиринта) или оставить это фишкой (Вылезающие за границу лабиринта комнаты????)

	//спавним комнату
	if (bShouldNotBeDeletedByChunksCheckPassed && bFlatCheckPassed)
	{
		FRoomState* RoomState = RoomsStates.Find(RoomPlaceHash);
		if (!RoomState)
		{
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red,
				                                 FString::Printf(TEXT("UMGMazeStateSubsystem::TryToSpawn called with invalid 'RoomPlaceHash' property: %d"), RoomPlaceHash));
			UE_LOG(LogTemp, Error, TEXT("UMGMazeStateSubsystem::TryToSpawn called with invalid 'RoomPlaceHash' property: %d"), RoomPlaceHash);
			return bSuccessfullySpawned;
		}

		RoomState->Level = RoomToSpawn.Room->Level;
		RoomState->DistanceMesh = RoomToSpawn.Room->DistanceMesh;
		RoomState->Offset = RoomToSpawn.OptionalOffset;
		RoomState->Rotation = FRotator(0.0f, Angle, 0.0f);
		for (auto RoomChunk : RoomToSpawn.Room->RoomChunks)
			RoomState->Chunks.Add(FRoomChunk(RoomChunk));
		if (RoomToSpawn.bIsFlat)
			RoomState->bIsFlat = true;
		if (RoomToSpawn.bIsFlat || RoomToSpawn.Room->RoomChunks.Num() > 1 || RoomToSpawn.SpawnType == NumberOfRooms)
			RoomState->bShouldNotBeDeletedByChunks = true;

		BiomeRoomsToRemoveFrom->RemoveAt(RootIndexInBiomeRooms);

		for (auto RotatedChunk_Temp : RotatedChunks_Temp)
			if (RotatedChunk_Temp != FVector2D(0.0f, 0.0f))
			{
				int32 Hash = GetRoomHash(GetWorld(), (RoomPlaceState.Coords.X / GridSize + RotatedChunk_Temp.X) * GridSize,
				                         (RoomPlaceState.Coords.Y / GridSize + RotatedChunk_Temp.Y) * GridSize);

				int32 RemovedItemsAmount = RoomsStates.Remove(Hash);
				if (RemovedItemsAmount < 1)
				{
					if (GEngine)
						GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "Error: Trying to remove an already removed or empty chunk in RoomsStates!");
					UE_LOG(LogTemp, Error, TEXT("Error: Trying to remove an already removed or empty chunk in RoomsStates!"));
				}
				//TODO удалить нужно из всех биомов а не только из текущего??? Или не надо????
				for (int32 i = 0; i < BiomeRoomsToRemoveFrom->Num(); i++)
					if (BiomeRoomsToRemoveFrom->Last(BiomeRoomsToRemoveFrom->Num() - 1 - i).Key == Hash)
					{
						BiomeRoomsToRemoveFrom->RemoveAt(i);
						break;
					}
			}

		bSuccessfullySpawned = true;
	}
	return bSuccessfullySpawned;
}

TArray<FVector2D> UMGMazeStateSubsystem::GetRotatedChunksArray(TArray<FVector2D>& ChunksToRotate, double Angle)
{
	TArray<FVector2D> Returning;
	for (auto ChunkToRotate : ChunksToRotate)
		Returning.Add(ChunkToRotate.GetRotated(Angle));

	return Returning;
}
