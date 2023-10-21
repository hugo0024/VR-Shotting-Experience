// Copyright 2021 LIV Inc. - MIT License
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LivBlueprintFunctionLibrary.generated.h"

class UTextureRenderTarget2D;
class UTexture2D;

/**
 * 
 */
UCLASS()
class LIV_API ULivBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintPure, Category = "Liv", meta = (WorldContext = "WorldContext"))
		static float GetMaxPixelAlphaValue(const UObject* WorldContext, UTexture2D* Texture);

	UFUNCTION(BlueprintPure, Category = "Liv", meta = (WorldContext = "WorldContext"))
		static float GetRenderTargetMaxPixelAlphaValue(UObject* WorldContext, UTextureRenderTarget2D* TextureRenderTarget);

	UFUNCTION(BlueprintPure, Category = "Liv", meta = (WorldContext = "WorldContext"))
		static float GetRenderTargetMaxDepthValue(UObject* WorldContext, UTextureRenderTarget2D* TextureRenderTarget);
};
