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
	TArray<AActor*> ReplaceInstancesWithActorsBySphereOverlap(FVector Center, float Radius);

	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGInstansedMeshActor")
	TArray<AActor*> ReplaceInstancesWithActorsByBoxOverlap(FBox Box);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "MazeGame|MGInstansedMeshActor")
	UHierarchicalInstancedStaticMeshComponent* HierarchicalInstancedMesh;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|MGInstansedMeshActor")
	TSubclassOf<AActor> ReplacementActor;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|MGInstancedMeshActor")
	float ReplacementActorZOffset;

protected:
	virtual void BeginPlay() override;
};
