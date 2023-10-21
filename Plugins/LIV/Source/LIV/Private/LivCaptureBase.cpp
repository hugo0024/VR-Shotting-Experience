// Copyright 2021 LIV Inc. - MIT License
#include "LivCaptureBase.h"

#include "BasePassRendering.h"
#include "Engine/World.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "LivConversions.h"
#include "LivShaders.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "LIV.h"
#include "LivPluginSettings.h"
#include "Windows/HideWindowsPlatformTypes.h"

#ifdef WITH_EDITORONLY//Editor only
#include "SNotificationList.h"
#include "NotificationManager.h"
#include "LivEditorSettings.h"
#endif

TAutoConsoleVariable<int32> CVarLivColorSpace(TEXT("Liv.ColorSpace"), 
	LIV_TEXTURE_COLOR_SPACE_SRGB,
	TEXT("Colorspace: Linear (1), sRGB (2)")
);

DEFINE_LOG_CATEGORY(LogLivCapture);

ULivCaptureBase::ULivCaptureBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bLivActive(false)
	, LivInputFrameWidth(0)
	, LivInputFrameHeight(0)
	, LivInputFrame(nullptr)
#if WITH_EDITORONLY_DATA
	, bRequestedCapture(false)
#endif

{
	PrimaryComponentTick.bCanEverTick = true;
}


struct LIV_InputFrame* ULivCaptureBase::GetInputFrame() const
{
	return LivInputFrame.Get();
}


bool ULivCaptureBase::IsLivCapturing() const
{
	return bLivActive;
}


FVector ULivCaptureBase::GetCameraLocation() const
{
	if (LivInputFrame)
	{
		return ConvertPosition<LIV_Vector3, FVector>(LivInputFrame->pose.local_position);
	}

	return FVector::ZeroVector;
}


FRotator ULivCaptureBase::GetCameraRotation() const
{
	if (LivInputFrame)
	{
		return Convert<LIV_Quaternion, FQuat>(LivInputFrame->pose.local_rotation).Rotator();
	}

	return FRotator::ZeroRotator;
}


FMatrix ULivCaptureBase::GetClipPlaneTransform() const
{
	if (LivInputFrame)
	{
		return Convert<LIV_Matrix4x4, FMatrix>(LivInputFrame->clipPlane.transform);
	}
	return FMatrix::Identity;
}

FVector ULivCaptureBase::GetClipPlaneLocation() const
{
	if (LivInputFrame)
	{
		const auto ClipPlaneTransform = Convert<LIV_Matrix4x4, FMatrix>(LivInputFrame->clipPlane.transform);
		return ClipPlaneTransform.TransformPosition(FVector::ZeroVector);
	}

	return FVector::ZeroVector;
}

FVector ULivCaptureBase::GetClipPlaneForward() const
{
	if (LivInputFrame)
	{
		const auto ClipPlaneTransform = Convert<LIV_Matrix4x4, FMatrix>(LivInputFrame->clipPlane.transform);
		return ClipPlaneTransform.TransformVector(FVector::ForwardVector);
	}

	return FVector::ForwardVector;
}

void ULivCaptureBase::OnActivated()
{
	// allocate an input frame 
	LivInputFrame = MakeShared<LIV_InputFrame>();

	// try to get input frame, else tear down
	if (!LIV_GetInputFrame(LivInputFrame.Get()))
	{
		LivInputFrame.Reset();

		UE_LOG(LogLivCapture, Warning, TEXT("LIV capture failed as unable to obtain input frame."));

		return;
	}

	// store expected render target dimensions
	LivInputFrameWidth = LivInputFrame->pose.width;
	LivInputFrameHeight = LivInputFrame->pose.height;

	// create render targets needed for rendering
	CreateRenderTargets();

	// track LIV is active
	bLivActive = true;

	// broadcast callback for when activated
	OnLivCaptureActivated.Broadcast();
}

void ULivCaptureBase::OnDeactivated()
{
	// free the input frame
	if (LivInputFrame != nullptr)
	{
		LivInputFrame.Reset();
	}

	// release render targets used for capture
	ReleaseRenderTargets();

	// track LIV inactive
	bLivActive = false;

	// broadcast callback for when deactivated
	OnLivCaptureDeactivated.Broadcast();
}

void ULivCaptureBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (bLivActive)
	{
		OnDeactivated();
	}
}

void ULivCaptureBase::CreateRenderTargets()
{
	// Implement in subclass	
}

void ULivCaptureBase::RecreateRenderTargets()
{
	ReleaseRenderTargets();
	CreateRenderTargets();
}

void ULivCaptureBase::ReleaseRenderTargets()
{
	// Implement in subclass	
}

void ULivCaptureBase::SetSceneCaptureComponentParameters(USceneCaptureComponent2D* InSceneCaptureComponent)
{
	const ULivPluginSettings* Settings = GetDefault<ULivPluginSettings>();
	
	if(Settings->bUseDebugCamera)
	{
		// set debug camera transform
		InSceneCaptureComponent->SetWorldLocation(Settings->DebugCameraWorldLocation);
		InSceneCaptureComponent->SetWorldRotation(Settings->DebugCameraWorldRotation);
		InSceneCaptureComponent->FOVAngle = Settings->DebugCameraHorizontalFOV;
	}
	else
	{
		// calculate camera transform
		const auto CameraPosition = ConvertPosition<LIV_Vector3, FVector>(LivInputFrame->pose.local_position);
		const auto CameraQuaternion = Convert<LIV_Quaternion, FQuat>(LivInputFrame->pose.local_rotation);
		const auto CameraRotation = CameraQuaternion.Rotator();

		// set camera transform
		InSceneCaptureComponent->SetRelativeLocation(CameraPosition);
		InSceneCaptureComponent->SetRelativeRotation(CameraRotation);

		// set field of view 
		InSceneCaptureComponent->FOVAngle = ConvertVerticalFOVToHorizontalFOV(
			LivInputFrame->pose.verticalFieldOfView,
			LivInputFrame->pose.width,
			LivInputFrame->pose.height);
	}
}

void ULivCaptureBase::Capture(const struct FLivCaptureContext& Context)
{
	// if not active, return early
	if (!bLivActive)
	{
		// editor auto-capture handling (see settings)
#if WITH_EDITORONLY_DATA
		if (!bRequestedCapture && GetMutableDefault<ULivPluginSettings>()->bAutoCaptureInEditor)
		{
			LIV_RequestCapture();
			bRequestedCapture = true;
		}
#endif
		return;
	}

	// get latest input frame
	if (!LIV_GetInputFrame(LivInputFrame.Get()))
	{
		// @TODO:	should we warn if we don't get any input frame? 
		//			shouldn't happen but don't want to cause log spam
		return;
	}

	// if cached dimensions don't match input frame dimensions, recreate render targets
	if ((LivInputFrameWidth != LivInputFrame->pose.width) || (LivInputFrameHeight != LivInputFrame->pose.height))
	{
		RecreateRenderTargets();
	}

	// Perform the actual scene capturing in subclass
}

void ULivCaptureBase::SubmitTextures(UTextureRenderTarget2D* BackgroundRenderTarget, UTextureRenderTarget2D* ForegroundRenderTarget)
{
	const ERHIFeatureLevel::Type FeatureLevel = GetWorld()->Scene->GetFeatureLevel();

	// Submit textures on render thread
	ENQUEUE_RENDER_COMMAND(LIVTextureSubmit)(
		[Background=BackgroundRenderTarget, Foreground= ForegroundRenderTarget, FeatureLevel]
			(FRHICommandListImmediate& RHICmdList)
		{
			
			const auto ForegroundRenderTargetResource = static_cast<FTextureRenderTarget2DResource*>(Foreground->Resource);
			const auto ForegroundTextureRHI = ForegroundRenderTargetResource->GetTextureRHI();

			if (!ForegroundTextureRHI) return; // Resource isn't quite ready yet!

			const auto ForegroundNativeTexture = static_cast<ID3D11Texture2D*>(ForegroundTextureRHI->GetNativeResource());

			LIV_Texture LivForegroundTexture{};
			LivForegroundTexture.type = LIV_TEXTURE_TYPE_COLOR_BUFFER;
			LivForegroundTexture.id = LIV_TEXTURE_FOREGROUND_COLOR_BUFFER_ID;
			LivForegroundTexture.dxgi_pixelFormat = GetRenderTargetFormat(ForegroundTextureRHI->GetFormat());
			LivForegroundTexture.d3d11_texturePtr = ForegroundNativeTexture;
			LivForegroundTexture.width = static_cast<int>(ForegroundTextureRHI->GetSizeX());
			LivForegroundTexture.height = -static_cast<int>(ForegroundTextureRHI->GetSizeY());
			LivForegroundTexture.colorSpace = CVarLivColorSpace.GetValueOnRenderThread(); // LIV_TEXTURE_COLOR_SPACE_LINEAR;

			const auto BackgroundRenderTargetResource = static_cast<FTextureRenderTarget2DResource*>(Background->Resource);
			const auto BackgroundTextureRHI = BackgroundRenderTargetResource->GetTextureRHI();

			if (!BackgroundTextureRHI) return; // Resource isn't quite ready yet!

			const auto BackgroundNativeTexture = static_cast<ID3D11Texture2D*>(BackgroundTextureRHI->GetNativeResource());

			LIV_Texture LivBackgroundTexture{};
			LivBackgroundTexture.type = LIV_TEXTURE_TYPE_COLOR_BUFFER;
			LivBackgroundTexture.id = LIV_TEXTURE_BACKGROUND_COLOR_BUFFER_ID;
			LivBackgroundTexture.dxgi_pixelFormat = GetRenderTargetFormat(BackgroundTextureRHI->GetFormat());
			LivBackgroundTexture.d3d11_texturePtr = BackgroundNativeTexture;
			LivBackgroundTexture.width = static_cast<int>(BackgroundTextureRHI->GetSizeX());
			LivBackgroundTexture.height = -static_cast<int>(BackgroundTextureRHI->GetSizeY());
			LivBackgroundTexture.colorSpace = CVarLivColorSpace.GetValueOnRenderThread(); // LIV_TEXTURE_COLOR_SPACE_LINEAR;

			LIV_AddTexture(&LivForegroundTexture);
			LIV_AddTexture(&LivBackgroundTexture);

			LIV_Submit();
		}
	);

	// @TODO: profile / eliminate. Semaphore / fence?
	FlushRenderingCommands();
}

UTextureRenderTarget2D* ULivCaptureBase::CreateRenderTarget2D(UObject* Owner,
	int32 Width,
	int32 Height,
	FName Name /*= NAME_None*/,
	ETextureRenderTargetFormat Format /*= ETextureRenderTargetFormat::RTF_RGBA8*/,
	FLinearColor ClearColor /*= FLinearColor::Black*/,
	float TargetGamma /*= 0.0f*/)
{
	/*UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);*/

	if (Width > 0 && Height > 0/* && World*/)
	{
		UTextureRenderTarget2D* NewRenderTarget2D = NewObject<UTextureRenderTarget2D>(GetTransientPackage(), Name);
		check(NewRenderTarget2D);
		NewRenderTarget2D->RenderTargetFormat = Format;
		NewRenderTarget2D->ClearColor = ClearColor;
		NewRenderTarget2D->bAutoGenerateMips = false;
		NewRenderTarget2D->TargetGamma = TargetGamma;
		NewRenderTarget2D->bForceLinearGamma = true;
		NewRenderTarget2D->InitAutoFormat(Width, Height);
		NewRenderTarget2D->UpdateResourceImmediate(true);

		return NewRenderTarget2D;
	}

	return nullptr;
}
