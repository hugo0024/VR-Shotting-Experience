// Copyright 2021 LIV Inc. - MIT License
#include "LivWorldSubsystem.h"
#include "IXRTrackingSystem.h"
#include "LivCaptureContext.h"
#include "LivCaptureMeshClipPlaneNoPostProcess.h"
#include "LivPluginSettings.h"

DEFINE_LOG_CATEGORY(LogLivWorldSubsystem);

ULivWorldSubsystem::ULivWorldSubsystem()
	: Super()
	, CameraRoot(nullptr)
	, CaptureComponent(nullptr)
	, TrackingOriginComponent(nullptr)
	, bAttached(false)
{
}

void ULivWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogLivWorldSubsystem, Log, TEXT("LIV World Subsystem Initialize (%s)."), *GetWorld()->GetName());
}

void ULivWorldSubsystem::Deinitialize()
{
	UE_LOG(LogLivWorldSubsystem, Log, TEXT("LIV World Subsystem Deinitialize (%s)."), *GetWorld()->GetName());

	DestroyCaptureResources();
}

TSubclassOf<ULivCaptureBase> ULivWorldSubsystem::GetCaptureComponentClass() const
{
	ULivPluginSettings* Settings = GetMutableDefault<ULivPluginSettings>();
	if (Settings && Settings->CaptureMethod)
	{
		return Settings->CaptureMethod;
	}
	
	return ULivCaptureMeshClipPlaneNoPostProcess::StaticClass();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
FTransform ULivWorldSubsystem::GetTrackingOriginTransform() const
{
	if (GEngine->XRSystem)
	{
		return GEngine->XRSystem->GetTrackingToWorldTransform();
	}

	return FTransform::Identity;
}

void ULivWorldSubsystem::SetTrackingOriginComponent(USceneComponent* NewTrackingOriginComponent)
{
	TrackingOriginComponent = NewTrackingOriginComponent;
	bAttached = false;
}

void ULivWorldSubsystem::Capture(FLivCaptureContext& Context)
{
	// check we have our resources - if world changed whilst still capturing we're in a new
	// system and have to create the resources again
	if(CaptureComponent == nullptr || CameraRoot == nullptr)
	{
		CreateCaptureResources();
	}
	
	UWorld* World = GetWorld();

	if (TrackingOriginComponent == nullptr)
	{
		if (World)
		{
			const FTransform TrackingTransform = GetTrackingOriginTransform();
			CameraRoot->SetWorldTransform(TrackingTransform, false, nullptr, ETeleportType::TeleportPhysics);
		}
		else
		{
			UE_LOG(LogLivWorldSubsystem, Warning, TEXT("No world present in Liv's world subsystem."));
		}
	}
	else if (!bAttached)
	{
		static const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
		CameraRoot->SetRelativeTransform(FTransform::Identity);
		CameraRoot->AttachToComponent(TrackingOriginComponent, AttachmentRules);

		bAttached = true;
	}

	CaptureComponent->Capture(Context);
}

void ULivWorldSubsystem::CreateCaptureResources()
{
	UWorld* World = GetWorld();
	
	// create the camera root that the capture component attaches to
	CameraRoot = NewObject<USceneComponent>(this, "LivCameraRoot");
	CameraRoot->RegisterComponentWithWorld(World);
	
	// create the capture component based on class set in settings
	const TSubclassOf<ULivCaptureBase> CaptureComponentClass = GetCaptureComponentClass();
	CaptureComponent = NewObject<ULivCaptureBase>(CameraRoot, CaptureComponentClass.Get());

	UE_LOG(LogLivWorldSubsystem, Log, TEXT("LIV World Subsystem : Using Capture Class (%s)."), *CaptureComponentClass->GetName());

	// setup/init the capture component
	static const FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
	CaptureComponent->RegisterComponentWithWorld(World);
	CaptureComponent->AttachToComponent(CameraRoot, AttachmentRules);
	CaptureComponent->OnActivated();
}

void ULivWorldSubsystem::DestroyCaptureResources()
{
	if (CaptureComponent)
	{
		CaptureComponent->OnDeactivated();
		CaptureComponent->DestroyComponent();
		CaptureComponent = nullptr;
	}

	if (CameraRoot)
	{
		CameraRoot->DestroyComponent();
		CameraRoot = nullptr;
	}
}


