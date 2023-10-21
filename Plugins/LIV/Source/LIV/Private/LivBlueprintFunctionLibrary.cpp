// Copyright 2021 LIV Inc. - MIT License
#include "LivBlueprintFunctionLibrary.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Math/UnrealMath.h"
#include "LivShaders.h"


float ULivBlueprintFunctionLibrary::GetMaxPixelAlphaValue(const UObject* WorldContext, UTexture2D* Texture)
{
	const FColor* Data = reinterpret_cast<const FColor*>(Texture->PlatformData->Mips[0].BulkData.LockReadOnly());

	const auto SizeX = Texture->GetSizeX();
	const auto SizeY = Texture->GetSizeY();

	float Max = 0;

	for (int32 X = 0; X < SizeX; X++)
	{
		for (int32 Y = 0; Y < SizeY; Y++)
		{
			FColor PixelColor = Data[Y * SizeX + X];
			if (PixelColor.A > Max)
			{
				Max = PixelColor.A;
			}
		}
	}

	return Max;
}

float ULivBlueprintFunctionLibrary::GetRenderTargetMaxPixelAlphaValue(UObject* WorldContext, UTextureRenderTarget2D* TextureRenderTarget)
{
	if (!TextureRenderTarget)
	{
		return -1.0f;
	}

	const auto Width = TextureRenderTarget->SizeX;
	const auto Height = TextureRenderTarget->SizeY;

	float Max = 0;

	EPixelFormat OutFormat = PF_Unknown;

	FTextureRenderTarget2DResource* RTResource = (FTextureRenderTarget2DResource*)TextureRenderTarget->GameThread_GetRenderTargetResource();
	if (!RTResource)
	{
		return -1.0f;
	}

	TArray<FColor> OutLDRValues;
	TArray<FLinearColor> OutHDRValues;

	FIntRect SampleRect(0, 0, Width, Height);
	FReadSurfaceDataFlags ReadSurfaceDataFlags;

	FRenderTarget* RenderTarget = TextureRenderTarget->GameThread_GetRenderTargetResource();
	OutFormat = TextureRenderTarget->GetFormat();

	const int32 NumPixelsToRead = Width * Height;

	switch (OutFormat)
	{
	case PF_B8G8R8A8:
		OutLDRValues.SetNumUninitialized(NumPixelsToRead);
		if (!RenderTarget->ReadPixelsPtr(OutLDRValues.GetData(), ReadSurfaceDataFlags, SampleRect))
		{
			OutFormat = PF_Unknown;
		}
		else
		{
			for (auto It = OutLDRValues.CreateConstIterator(); It; ++It) 
			{
				if (It->A > Max)
				{
					Max = It->A;
				}
			}
		}
		break;
	case PF_FloatRGBA:
		OutHDRValues.SetNumUninitialized(NumPixelsToRead);
		if (!RenderTarget->ReadLinearColorPixelsPtr(OutHDRValues.GetData(), ReadSurfaceDataFlags, SampleRect))
		{
			OutFormat = PF_Unknown;
		}
		else
		{
			for (auto It = OutHDRValues.CreateConstIterator(); It; ++It)
			{
				if (It->A > Max)
				{
					Max = It->A;
				}
			}
		}
		break;
	default:
		OutFormat = PF_Unknown;
		break;
	}
	
	return Max;
}

float ULivBlueprintFunctionLibrary::GetRenderTargetMaxDepthValue(UObject* WorldContext, UTextureRenderTarget2D* TextureRenderTarget)
{
	if (!TextureRenderTarget)
	{
		return -1.0f;
	}

	const auto Width = TextureRenderTarget->SizeX;
	const auto Height = TextureRenderTarget->SizeY;

	float Max = 0;

	EPixelFormat OutFormat = PF_Unknown;

	FTextureRenderTarget2DResource* RTResource = (FTextureRenderTarget2DResource*)TextureRenderTarget->GameThread_GetRenderTargetResource();
	if (!RTResource)
	{
		return -1.0f;
	}

	TArray<FColor> OutLDRValues;
	TArray<FLinearColor> OutHDRValues;

	FIntRect SampleRect(0, 0, Width, Height);
	FReadSurfaceDataFlags ReadSurfaceDataFlags;

	FRenderTarget* RenderTarget = TextureRenderTarget->GameThread_GetRenderTargetResource();
	OutFormat = TextureRenderTarget->GetFormat();

	const int32 NumPixelsToRead = Width * Height;

	switch (OutFormat)
	{
	case PF_B8G8R8A8:
		OutLDRValues.SetNumUninitialized(NumPixelsToRead);
		if (!RenderTarget->ReadPixelsPtr(OutLDRValues.GetData(), ReadSurfaceDataFlags, SampleRect))
		{
			OutFormat = PF_Unknown;
		}
		else
		{
			for (auto It = OutLDRValues.CreateConstIterator(); It; ++It)
			{
				if (It->R > Max)
				{
					Max = It->R;
				}
			}
		}
		break;
	case PF_FloatRGBA:
		OutHDRValues.SetNumUninitialized(NumPixelsToRead);
		if (!RenderTarget->ReadLinearColorPixelsPtr(OutHDRValues.GetData(), ReadSurfaceDataFlags, SampleRect))
		{
			OutFormat = PF_Unknown;
		}
		else
		{
			for (auto It = OutHDRValues.CreateConstIterator(); It; ++It)
			{
				if (It->R > Max)
				{
					Max = It->R;
				}
			}
		}
		break;
	default:
		OutFormat = PF_Unknown;
		break;
	}

	return Max;
}
