// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGInstancedMeshActorStatic.generated.h"

UCLASS()
class MAZEGAME_API AMGInstancedMeshActorStatic : public AActor
{
	GENERATED_BODY()
	
public:	
	AMGInstancedMeshActorStatic();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "MazeGame|MGInstansedMeshActorStatic")
	UHierarchicalInstancedStaticMeshComponent* HierarchicalInstancedMesh;

protected:
	virtual void BeginPlay() override;
};
