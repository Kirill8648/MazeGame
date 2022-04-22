// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/MGInstancedMeshActorStatic.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"

AMGInstancedMeshActorStatic::AMGInstancedMeshActorStatic()
{
	PrimaryActorTick.bCanEverTick = false;
	HierarchicalInstancedMesh = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(FName("HierarchicalInstancedMesh"));
	HierarchicalInstancedMesh->SetupAttachment(RootComponent);
	HierarchicalInstancedMesh->SetGenerateOverlapEvents(false);
	HierarchicalInstancedMesh->CanCharacterStepUpOn = ECB_No;
}
