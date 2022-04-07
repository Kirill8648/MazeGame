// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/MGInstancedMeshActor.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"

AMGInstancedMeshActor::AMGInstancedMeshActor()
{
	PrimaryActorTick.bCanEverTick = false;
	HierarchicalInstancedMesh = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HierarchicalInstancedMesh"));
	HierarchicalInstancedMesh->SetupAttachment(RootComponent);
	HierarchicalInstancedMesh->SetGenerateOverlapEvents(false);
	HierarchicalInstancedMesh->CanCharacterStepUpOn = ECB_No;
}

TArray<AActor*> AMGInstancedMeshActor::ReplaceInstancesWithActorsBySphereOverlap(FVector Center, float Radius)
{
	TArray<AActor*> SpawnedActors;

	TArray<int32> OverlappedIndexes = HierarchicalInstancedMesh->GetInstancesOverlappingSphere(Center, Radius);
	for (auto& OverlappedIndex : OverlappedIndexes/*HierarchicalInstancedMesh->GetInstancesOverlappingSphere(Center, Radius)*/)
	{
		FTransform InstTransform;
		HierarchicalInstancedMesh->GetInstanceTransform(OverlappedIndex, InstTransform, true);
		InstTransform.SetLocation(FVector(InstTransform.GetLocation().X, InstTransform.GetLocation().Y, InstTransform.GetLocation().Z + ReplacementActorZOffset));
		//HierarchicalInstancedMesh->RemoveInstance(OverlappedIndex);
		SpawnedActors.Emplace(GetWorld()->SpawnActor<AActor>(ReplacementActor, InstTransform));
	}
	HierarchicalInstancedMesh->RemoveInstances(OverlappedIndexes);

	return SpawnedActors;
}

TArray<AActor*> AMGInstancedMeshActor::ReplaceInstancesWithActorsByBoxOverlap(FBox Box)
{
	TArray<AActor*> SpawnedActors;

	TArray<int32> OverlappedIndexes = HierarchicalInstancedMesh->GetInstancesOverlappingBox(Box);
	for (auto& OverlappedIndex : OverlappedIndexes/*HierarchicalInstancedMesh->GetInstancesOverlappingBox(Box)*/)
	{
		FTransform InstTransform;
		HierarchicalInstancedMesh->GetInstanceTransform(OverlappedIndex, InstTransform, true);
		//HierarchicalInstancedMesh->RemoveInstance(OverlappedIndex);
		SpawnedActors.Emplace(GetWorld()->SpawnActor<AActor>(ReplacementActor, InstTransform));
	}
	HierarchicalInstancedMesh->RemoveInstances(OverlappedIndexes);

	return SpawnedActors;
}


void AMGInstancedMeshActor::BeginPlay()
{
	Super::BeginPlay();
}
