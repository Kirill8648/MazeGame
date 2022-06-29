// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/Generation/MGMazeStateSubsystem.h"

#include "MGPlayerDataSubsystem.h"
#include "Kismet/GameplayStatics.h"

int32 UMGMazeStateSubsystem::GetRoomObjectHash(AActor* Actor)
{
	int32 Hash = 0;
	if (const UMGPlayerDataSubsystem* PlayerDataSubsystem = UGameplayStatics::GetGameInstance(Actor->GetWorld())->GetSubsystem<UMGPlayerDataSubsystem>();
		PlayerDataSubsystem && Actor->Tags.Num() > 0)
	{
		const float GridSize = PlayerDataSubsystem->CurrentMazeCellSize / 2;
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
		const float GridSize = PlayerDataSubsystem->CurrentMazeCellSize / 2;
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
