// Copyright 2021 LIV Inc. - MIT License
#include "LivPluginSettings.h"
#include "LivCaptureMeshClipPlanePostProcess.h"

ULivPluginSettings::ULivPluginSettings()
	: CaptureMethod(ULivCaptureMeshClipPlanePostProcess::StaticClass())
	, bUseDebugCamera(false)
	, DebugCameraHorizontalFOV(90.0f)
#if WITH_EDITOR
	, bAutoCaptureInEditor(false)
#endif
{

}

#if WITH_EDITOR
void ULivPluginSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ULivPluginSettings, CaptureMethod))
	{
		if(CaptureMethod == nullptr)
		{
			CaptureMethod = ULivCaptureMeshClipPlanePostProcess::StaticClass();
		}
	}
}
#endif
