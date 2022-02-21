// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/MGInstancedMeshActor.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"

AMGInstancedMeshActor::AMGInstancedMeshActor()
{
	PrimaryActorTick.bCanEverTick = false;
	HierarchicalInstancedMesh = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HierarchicalInstancedMesh"));
	HierarchicalInstancedMesh->SetupAttachment(RootComponent);
}

TArray<AActor*> AMGInstancedMeshActor::ReplaceInstancesWithActorsBySphereOverlap()
{
}

TArray<AActor*> AMGInstancedMeshActor::ReplaceInstancesWithActorsByBoxOverlap()
{
}


void AMGInstancedMeshActor::BeginPlay()
{
	Super::BeginPlay();
}

