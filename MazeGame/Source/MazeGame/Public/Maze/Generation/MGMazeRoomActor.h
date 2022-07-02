// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGMazeRoomActor.generated.h"

UCLASS()
class MAZEGAME_API AMGMazeRoomActor : public AActor
{
	GENERATED_BODY()
	
public:
	AMGMazeRoomActor();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DistantMesh;

	TSoftObjectPtr<UWorld> LevelToLoad;
};
