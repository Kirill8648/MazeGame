// Fill out your copyright notice in the Description page of Project Settings.


#include "MGBlueprintFunctionLibrary.h"

#include "Engine/TextureCube.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/TextureRenderTargetCube.h"
#include "Kismet/GameplayStatics.h"

FString UMGBlueprintFunctionLibrary::SanitizeFloat(double InFloat, const int32 InMinFractionalDigits)
{
	return FString::SanitizeFloat(InFloat, InMinFractionalDigits);
}

void UMGBlueprintFunctionLibrary::SpawnActorsInGrid(UObject* WorldContextObject, const TSubclassOf<AActor> ActorClass, const int32 X, const int32 Y,
                                                    const float GridSize)
{
	if (ActorClass && WorldContextObject)
	{
		UWorld* World = WorldContextObject->GetWorld();
		for (int32 i = 0; i < X; i++)
			for (int32 j = 0; j < Y; j++)
				World->SpawnActor<AActor>(ActorClass, FVector(i * GridSize, j * GridSize, 0.0f), FRotator());
	}
}

void UMGBlueprintFunctionLibrary::DestroyAllActorsByClass(UObject* WorldContextObject, const TSubclassOf<AActor> ActorClass)
{
	if (ActorClass && WorldContextObject)
	{
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(WorldContextObject, ActorClass, Actors);
		for (const auto Actor : Actors)
			Actor->Destroy();
	}
}

UTexture2D* UMGBlueprintFunctionLibrary::GetTextureFromRenderTarget2D(UTextureRenderTarget2D* TextureRenderTarget)
{
	// Creates Texture2D to store TextureRenderTarget content
	UTexture2D* Texture = UTexture2D::CreateTransient(TextureRenderTarget->SizeX, TextureRenderTarget->SizeY, PF_B8G8R8A8);
#if WITH_EDITORONLY_DATA
	Texture->MipGenSettings = TMGS_NoMipmaps;
#endif
	Texture->SRGB = TextureRenderTarget->SRGB;

	// Read the pixels from the RenderTarget and store them in a FColor array
	TArray<FColor> SurfData;
	FRenderTarget* RenderTarget = TextureRenderTarget->GameThread_GetRenderTargetResource();
	RenderTarget->ReadPixels(SurfData);

	// Lock and copies the data between the textures
	void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	const int32 TextureDataSize = SurfData.Num() * 4;
	FMemory::Memcpy(TextureData, SurfData.GetData(), TextureDataSize);
	Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
	// Apply Texture changes to GPU memory
	Texture->UpdateResource();

	return Texture;
}

UTextureCube* UMGBlueprintFunctionLibrary::GetTextureFromRenderTargetCube(UTextureRenderTargetCube* TextureRenderTargetCube)
{
	/*UTextureCube* Texture = nullptr;
	TextureRenderTargetCube->ConstructTextureCube(Texture, "Name", EObjectFlags::RF_Standalone/*RF_WillBeLoaded#1#);

	if (!Texture)
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, "This shit is nullptr");
	return Texture;*/

	// Creates TextureCube to store TextureRenderTarget content
	UTextureCube* Texture = UTextureCube::CreateTransient(TextureRenderTargetCube->SizeX, TextureRenderTargetCube->SizeX * 6, PF_B8G8R8A8);
#if WITH_EDITORONLY_DATA
	Texture->MipGenSettings = TMGS_NoMipmaps;
#endif
	Texture->SRGB = TextureRenderTargetCube->SRGB;

	// Read the pixels from the RenderTarget and store them in a FColor array
	TArray<FColor> SurfData;
	FRenderTarget* RenderTarget = TextureRenderTargetCube->GameThread_GetRenderTargetResource();
	RenderTarget->ReadPixels(SurfData);

	// Lock and copies the data between the textures
	void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	const int32 TextureDataSize = SurfData.Num() * 4;
	FMemory::Memcpy(TextureData, SurfData.GetData(), TextureDataSize);
	Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
	// Apply Texture changes to GPU memory
	Texture->UpdateResource();

	return Texture;
}
