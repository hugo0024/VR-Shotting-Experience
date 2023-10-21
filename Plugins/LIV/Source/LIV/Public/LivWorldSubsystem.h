// Copyright 2021 LIV Inc. - MIT License
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "LivWorldSubsystem.generated.h"

class ULivCaptureBase;
DECLARE_LOG_CATEGORY_EXTERN(LogLivWorldSubsystem, Log, Log);

/**
 * 
 */
UCLASS()
class LIV_API ULivWorldSubsystem : public UWorldSubsystem
{
public:
	
	GENERATED_BODY()

	ULivWorldSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintPure)
		TSubclassOf<ULivCaptureBase> GetCaptureComponentClass() const;

	UFUNCTION(BlueprintPure)
		FTransform GetTrackingOriginTransform() const;

	UFUNCTION(BlueprintPure)
		ULivCaptureBase* GetCaptureComponent() const { return CaptureComponent; }

	UFUNCTION(BlueprintPure)
		USceneComponent* GetCameraRoot() const { return CameraRoot; }

	UFUNCTION(BlueprintCallable)
		void SetTrackingOriginComponent(USceneComponent* NewTrackingOriginComponent);

	void Capture(struct FLivCaptureContext& Context);

	void CreateCaptureResources();

	void DestroyCaptureResources();

private:

	UPROPERTY(Transient)
		USceneComponent* CameraRoot;
	
	UPROPERTY(Transient)
		ULivCaptureBase* CaptureComponent;

	UPROPERTY(Transient)
		USceneComponent* TrackingOriginComponent;

	bool bAttached;
};
