// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MGInstancedMeshActorStatic.h"
#include "GameFramework/Actor.h"
#include "MGInstancedMeshActor.generated.h"

/**
 * Класс, содержащий объект динамических экземпляров стен
 */
UCLASS()
class MAZEGAME_API AMGInstancedMeshActor : public AMGInstancedMeshActorStatic
{
	GENERATED_BODY()

public:
	/**
	* Стандартный конструктор.
	*/
	AMGInstancedMeshActor();

	/**
	* Заменяет экземпляры стен, которые пересекаются со сферой, на акторы.
	* @param Center - центр сферы в мировых координтах
	* @param Radius - радиус сферы
	* @return - массив указателей на заспавненные акторы
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGInstansedMeshActor")
	TArray<AActor*> ReplaceInstancesWithActorsBySphereOverlap(FVector Center, float Radius);

	/**
	* Заменяет экземпляры стен, которые пересекаются с коробкой, на акторы.
	* @param Box - структура коробки
	* @return - массив указателей на заспавненные акторы
	*/
	UFUNCTION(BlueprintCallable, Category = "MazeGame|MGInstansedMeshActor")
	TArray<AActor*> ReplaceInstancesWithActorsByBoxOverlap(FBox Box);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|MGInstansedMeshActor")
	TSubclassOf<AActor> ReplacementActor;/**< Класс актора, который нужно заспавнить при замене. */

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MazeGame|MGInstancedMeshActor")
	float ReplacementActorZOffset;/**< Корректировка позиции актора при спавне. */

protected:
	/**
	* Функция, вызывающаяся в момент начала игры.
	*/
	virtual void BeginPlay() override;
};
