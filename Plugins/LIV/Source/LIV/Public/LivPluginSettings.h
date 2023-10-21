// Copyright 2021 LIV Inc. - MIT License
#pragma once

#include "CoreMinimal.h"
#include "Engine/Scene.h"
#include "LivPluginSettings.generated.h"

/**
 * Liv plugin settings
 */
UCLASS(Config = Engine, DefaultConfig)
class LIV_API ULivPluginSettings : public UObject
{
	GENERATED_BODY()
	
public:

	ULivPluginSettings();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	/**
	 * Choose a subclass of ULivCaptureBase to select which rendering
	 * technique the LIV plugin will use to capture gameplay.
	 */
	UPROPERTY(config, EditAnywhere, Category = "Liv")
		TSubclassOf<class ULivCaptureBase> CaptureMethod;

	/**
	 * Post process setting that will only apply to LIV output (if post processing
	 * is supported for current capture method).
	 */
	UPROPERTY(config, EditAnywhere, Category = "Liv")
		FPostProcessSettings PostProcessSettings;

	/**
	 * Debugging Settings
	 */

	/**
	 * If enabled use debug settings values rather than setting them from LIV data.
	 */
	UPROPERTY(config, EditAnywhere, Category = "Liv|Debug")
		bool bUseDebugCamera;

	/**
	 * If bUseDebugCamera is enabled use this world location for the camera.
	 */
	UPROPERTY(config, EditAnywhere, Category = "Liv|Debug")
		FVector DebugCameraWorldLocation;

	/**
	 * If bUseDebugCamera is enabled use this world rotation for the camera.
	 */
	UPROPERTY(config, EditAnywhere, Category = "Liv|Debug")
		FRotator DebugCameraWorldRotation;

	/**
	 * If bUseDebugCamera is enabled use this (horizontal) field of view for the camera.
	 */
	UPROPERTY(config, EditAnywhere, Category = "Liv|Debug", meta = (DisplayName = "Horizontal FOV", UIMin = "5.0", UIMax = "170", ClampMin = "0.001", ClampMax = "360.0"))
		float DebugCameraHorizontalFOV;
	

#if WITH_EDITORONLY_DATA

	/**
	 * If enabled a message will be sent to request LIV
	 * capture out process automatically when developing in editor.
	 */
	UPROPERTY(config, EditAnywhere, Category = "Liv", DisplayName = "Automatic Capture in Editor")
		bool bAutoCaptureInEditor;
	
#endif

};
