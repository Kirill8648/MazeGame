// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGInstancedMeshActor.generated.h"

UCLASS()
class MAZEGAME_API AMGInstancedMeshActor : public AActor
{
	GENERATED_BODY()

public:
	AMGInstancedMeshActor();

	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGInstansedMeshActor")
	TArray<AActor*> ReplaceInstancesWithActorsBySphereOverlap();

	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGInstansedMeshActor")
	TArray<AActor*> ReplaceInstancesWithActorsByBoxOverlap();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MazeGame|MGInstansedMeshActor")
	UHierarchicalInstancedStaticMeshComponent* HierarchicalInstancedMesh;

protected:
	virtual void BeginPlay() override;
};
