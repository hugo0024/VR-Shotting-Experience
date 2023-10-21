// Copyright 2021 LIV Inc. - MIT License
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "LivCaptureContext.h"
#include "Engine/LocalPlayer.h"
#include "LivLocalPlayerSubsystem.generated.h"

class ULivWorldSubsystem;
class ULivCaptureBase;

DECLARE_LOG_CATEGORY_EXTERN(LogLivLocalPlayerSubsystem, Log, Log);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLivLocalPlayerActivationDelegate);

/**
 * 
 */
UCLASS()
class LIV_API ULivLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
public:

	GENERATED_BODY()

	ULivLocalPlayerSubsystem();
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintPure, meta=(DisplayName="GetLocalPlayer"))
		ULocalPlayer* GetLocalPlayerBP() const { return GetLocalPlayer<ULocalPlayer>(); }

	UFUNCTION(BlueprintPure)
		ULivWorldSubsystem* GetLivWorldSubsystem() const;

	UFUNCTION(BlueprintPure)
		TSubclassOf<ULivCaptureBase> GetCaptureComponentClass() const;

	UFUNCTION(BlueprintPure)
		FTransform GetTrackingOriginTransform() const;

	/**
	 * True is LIV has requested capture and we have acknowledge and setup
	 * the necessary resources for capture.
	 */
	UFUNCTION(BlueprintPure)
		bool IsCaptureActive() const;

	/**
	 * Shutdown LIV capture and try to reconnect and recreate capture resources.
	 */
	UFUNCTION(BlueprintCallable)
		void ResetCapture();

	/**
	 * Hide a component from rendering during LIV scene capture.
	 */
	UFUNCTION(BlueprintCallable)
		void HideComponent(UPrimitiveComponent* InComponent);

	/**
	 * Stop hiding a component previously hidden with HideComponent.
	 */
	UFUNCTION(BlueprintCallable)
		void ShowComponent(UPrimitiveComponent* InComponent);

	/**
	 * Clear components previously hidden with HideComponent.
	 */
	UFUNCTION(BlueprintCallable)
		void ClearHiddenComponents();

	/**
	 * Hide an actor from rendering during LIV scene capture.
	 */
	UFUNCTION(BlueprintCallable)
		void HideActor(AActor* InActor);

	/**
	 * Stop hiding an actor previously hidden with HideComponent.
	 */
	UFUNCTION(BlueprintCallable)
		void ShowActor(AActor* InActor);

	/**
	 * Clear actors previously hidden with HideComponent.
	 */
	UFUNCTION(BlueprintCallable)
		void ClearHiddenActors();

public:

	UPROPERTY(BlueprintAssignable)
		FLivLocalPlayerActivationDelegate OnCaptureActivated;

	UPROPERTY(BlueprintAssignable)
		FLivLocalPlayerActivationDelegate OnCaptureDeactivated;

	UPROPERTY()
		FLivCaptureContext CaptureContext;

protected:
	
	void HandleLivConnection();
	void LivCaptureActivated();
	void LivCaptureDeactivated();
	bool Tick(float DeltaTime);

private:
	
	FDelegateHandle TickHandle;
	bool bLivActive {false};
	TSharedPtr<struct LIV_InputFrame> LivInputFrame;
	int32 LivInputFrameWidth {0};
	int32 LivInputFrameHeight {0};
};
