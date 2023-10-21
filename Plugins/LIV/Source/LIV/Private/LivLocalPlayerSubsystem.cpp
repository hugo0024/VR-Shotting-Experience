// Copyright 2021 LIV Inc. - MIT License
#include "LivLocalPlayerSubsystem.h"
#include "LivCaptureMeshClipPlaneNoPostProcess.h"
#include "LivPluginSettings.h"
#include "LivModule.h"
#include "LivWorldSubsystem.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "IXRTrackingSystem.h"
#include "Containers/Ticker.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "LIV.h"
#include "Windows/HideWindowsPlatformTypes.h"

class ULivPluginSettings;
DEFINE_LOG_CATEGORY(LogLivLocalPlayerSubsystem);

static TWeakObjectPtr<ULivLocalPlayerSubsystem> MainLocalPlayerSubsystem { nullptr };

ULivLocalPlayerSubsystem::ULivLocalPlayerSubsystem()
	: Super()
{
}

bool ULivLocalPlayerSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	ILivModule* LivModule = FModuleManager::GetModulePtr<ILivModule>("LIV");
	return LivModule && LivModule->IsSDKLoaded();
}

void ULivLocalPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	ULocalPlayer* LocalPlayer = GetLocalPlayer<ULocalPlayer>();

	UE_LOG(LogLivLocalPlayerSubsystem, Log, TEXT("LIV Local Player Subsystem Initialize (%s-%d Primary=%s)."),
		*LocalPlayer->GetName(),
		LocalPlayer->GetControllerId(),
		LocalPlayer->IsPrimaryPlayer() ? TEXT("True") : TEXT("False"));
	
	// @TODO: using this as a workaround to ensure only one subsystem will send frames
	//		  IsPrimaryPlayer always returns true it seems 
	if (!MainLocalPlayerSubsystem.IsValid())
	{
		MainLocalPlayerSubsystem = this;
		
		// bind a tick
		TickHandle = FTicker::GetCoreTicker().AddTicker(
			FTickerDelegate::CreateUObject(this, &ULivLocalPlayerSubsystem::Tick)
		);
	}
}

void ULivLocalPlayerSubsystem::Deinitialize()
{
	ULocalPlayer* LocalPlayer = GetLocalPlayer<ULocalPlayer>();
	
	UE_LOG(LogLivLocalPlayerSubsystem, Log, TEXT("LIV Local Player Subsystem Deinitialize (%s-%d Primary=%s)."),
		*LocalPlayer->GetName(),
		LocalPlayer->GetControllerId(),
		LocalPlayer->IsPrimaryPlayer() ? TEXT("True") : TEXT("False"));

	// unbind tick
	if (TickHandle.IsValid())
	{
		FTicker::GetCoreTicker().RemoveTicker(TickHandle);
		TickHandle.Reset();
	}
	
	LivCaptureDeactivated();
}

ULivWorldSubsystem* ULivLocalPlayerSubsystem::GetLivWorldSubsystem() const
{
	if(ULocalPlayer* LocalPlayer = GetLocalPlayer<ULocalPlayer>())
	{
		if(APlayerController* PlayerController = LocalPlayer->PlayerController)
		{
			if(UWorld* World = PlayerController->GetWorld())
			{
				return World->GetSubsystem<ULivWorldSubsystem>();
			}
			else
			{
				UE_LOG(LogLivLocalPlayerSubsystem, Warning, TEXT("Cannot activate LIV capture as its player controller is not in a valid world."));
			}
		}
		else
		{
			UE_LOG(LogLivLocalPlayerSubsystem, Warning, TEXT("Cannot activate LIV capture as no local player has no player controller."));
		}
	}
	else
	{
		UE_LOG(LogLivLocalPlayerSubsystem, Warning, TEXT("Cannot activate LIV capture as no local player is present."));
	}

	return nullptr;
}

TSubclassOf<ULivCaptureBase> ULivLocalPlayerSubsystem::GetCaptureComponentClass() const
{
	ULivPluginSettings* Settings = GetMutableDefault<ULivPluginSettings>();
	if (Settings && Settings->CaptureMethod)
	{
		return Settings->CaptureMethod;
	}
	return ULivCaptureMeshClipPlaneNoPostProcess::StaticClass();
}

FTransform ULivLocalPlayerSubsystem::GetTrackingOriginTransform() const
{
	if (GEngine->XRSystem)
	{
		return GEngine->XRSystem->GetTrackingToWorldTransform();
	}

	return FTransform::Identity;
}

bool ULivLocalPlayerSubsystem::IsCaptureActive() const
{
	return bLivActive;
}

void ULivLocalPlayerSubsystem::ResetCapture()
{
	// if capturing, call deactivate
	if(IsCaptureActive())
	{
		// stop capturing release resources
		LivCaptureDeactivated();
		
		// recheck connection this frame (should reconnect & recreate resources)
		HandleLivConnection();
	}
}


void ULivLocalPlayerSubsystem::HideComponent(UPrimitiveComponent* InComponent)
{
	if (InComponent)
	{
		CaptureContext.HiddenComponents.AddUnique(InComponent);
	}
}

void ULivLocalPlayerSubsystem::ShowComponent(UPrimitiveComponent* InComponent)
{
	if (InComponent)
	{
		CaptureContext.HiddenComponents.Remove(InComponent);
	}
}

void ULivLocalPlayerSubsystem::ClearHiddenComponents()
{
	CaptureContext.HiddenComponents.Reset();
}

void ULivLocalPlayerSubsystem::HideActor(AActor* InActor)
{
	if (InActor)
	{
		CaptureContext.HiddenActors.AddUnique(InActor);
	}
}

void ULivLocalPlayerSubsystem::ShowActor(AActor* InActor)
{
	if (InActor)
	{
		CaptureContext.HiddenActors.Remove(InActor);
	}
}

void ULivLocalPlayerSubsystem::ClearHiddenActors()
{
	CaptureContext.HiddenActors.Reset();
}

void ULivLocalPlayerSubsystem::HandleLivConnection()
{
	if (!bLivActive)
	{
		if (LIV_IsActive())
		{
			UE_LOG(LogLivLocalPlayerSubsystem, Log, TEXT("LIV capture started."));

			// handle activation internally
			LivCaptureActivated();
		}
	}
	else
	{
		if (!LIV_IsActive())
		{
			UE_LOG(LogLivLocalPlayerSubsystem, Log, TEXT("LIV capture stopped."));

			// handle deactivation internally
			LivCaptureDeactivated();
		}
	}
}

void ULivLocalPlayerSubsystem::LivCaptureActivated()
{
	// allocate an input frame 
	LivInputFrame = MakeShared<LIV_InputFrame>();

	// try to get input frame, else tear down
	if (!LIV_GetInputFrame(LivInputFrame.Get()))
	{
		LivInputFrame.Reset();

		UE_LOG(LogLivLocalPlayerSubsystem, Warning, TEXT("LIV capture failed as unable to obtain input frame."));

		return;
	}

	// store expected render target dimensions
	LivInputFrameWidth = LivInputFrame->pose.width;
	LivInputFrameHeight = LivInputFrame->pose.height;
	
	// Create the resources we need for capturing within the world subsystem
	ULivWorldSubsystem* LivWorldSubsystem = GetLivWorldSubsystem();
	if(!LivWorldSubsystem)
	{
		LivCaptureDeactivated();
		return;
	}
	LivWorldSubsystem->CreateCaptureResources();

	// signal that we will be sending frames
	LIV_Start();

	// track LIV is active
	bLivActive = true;

	// broadcast subsystem event
	OnCaptureActivated.Broadcast();

	// broadcast module event
	ILivModule::Get().OnLivCaptureActivated().Broadcast();
}

void ULivLocalPlayerSubsystem::LivCaptureDeactivated()
{
	// explicitly deallocate the input frame
	LivInputFrame.Reset();

	// if there is a world sub system (should be), destroy the resources
	if (ULivWorldSubsystem* LivWorldSubsystem = GetLivWorldSubsystem())
	{
		LivWorldSubsystem->DestroyCaptureResources();
	}

	// track LIV inactive
	bLivActive = false;

	// broadcast subsystem event
	OnCaptureDeactivated.Broadcast();

	// broadcast module event
	ILivModule::Get().OnLivCaptureDeactivated().Broadcast();
}

bool ULivLocalPlayerSubsystem::Tick(float DeltaTime)
{
	HandleLivConnection();

	// not active, nothing to do
	if(!bLivActive)
	{
		return true;
	}

	if (ULivWorldSubsystem* LivWorldSubsystem = GetLivWorldSubsystem())
	{
		// capture this frame through world subsystem
		LivWorldSubsystem->Capture(CaptureContext);
	}

	// @TODO: warn if not world subsystem? May just cause log spam between loading levels.
	
	return true;
}
