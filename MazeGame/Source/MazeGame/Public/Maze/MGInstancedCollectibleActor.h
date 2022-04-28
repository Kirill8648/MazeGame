// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MGInstancedMeshActor.h"
#include "MGInstancedCollectibleActor.generated.h"

/**
 * Класс, содержащий объект экземпляров собираемых предметов.
 */
UCLASS()
class MAZEGAME_API AMGInstancedCollectibleActor : public AMGInstancedMeshActorStatic
{
	GENERATED_BODY()
	
public:
	/**
	* Стандартный конструктор.
	*/
	AMGInstancedCollectibleActor();
};
