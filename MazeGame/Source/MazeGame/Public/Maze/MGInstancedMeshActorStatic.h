// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MGInstancedMeshActorStatic.generated.h"

/**
 * Класс, содержащий объект статических экземпляров стен
 */
UCLASS()
class MAZEGAME_API AMGInstancedMeshActorStatic : public AActor
{
	GENERATED_BODY()
	
public:
	/**
	* Стандартный конструктор.
	*/
	AMGInstancedMeshActorStatic();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "MazeGame|MGInstansedMeshActorStatic")
	UHierarchicalInstancedStaticMeshComponent* HierarchicalInstancedMesh;/**< Компонент меша, содержащего много экземпляров. */
};
