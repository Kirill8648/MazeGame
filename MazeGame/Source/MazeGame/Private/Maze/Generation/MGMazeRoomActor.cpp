// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/Generation/MGMazeRoomActor.h"

AMGMazeRoomActor::AMGMazeRoomActor()
{
	PrimaryActorTick.bCanEverTick = false;
	DistantMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("DistantMesh"));
	SetRootComponent(DistantMesh);
	//DistantMesh->SetupAttachment(RootComponent);
}
