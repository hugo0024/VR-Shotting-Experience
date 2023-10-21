// Copyright 2021 LIV Inc. - MIT License
#include "LivShaders.h"
#include "RHIStaticStates.h"
#include "ShaderParameterUtils.h"
#include "RenderGraphBuilder.h"

IMPLEMENT_SHADER_TYPE(, FLivSingleTextureVertexShader, TEXT("/Plugin/Liv/LivSingleTextureVS.usf"), TEXT("MainVS"), SF_Vertex)
IMPLEMENT_SHADER_TYPE(, FLivInvertAlphaPixelShader, TEXT("/Plugin/Liv/LivInvertAlphaPS.usf"), TEXT("MainPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, FLivCopyPixelShader, TEXT("/Plugin/Liv/LivCopyPS.usf"), TEXT("MainPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, FLivCombineAlphaShader, TEXT("/Plugin/Liv/LivCombineAlphaPS.usf"), TEXT("MainPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, FLivForegroundSegmentationPixelShader, TEXT("/Plugin/Liv/LivForegroundSegmentationPS.usf"), TEXT("MainPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, FLivForegroundSegmentationPostProcessedPixelShader, TEXT("/Plugin/Liv/LivForegroundSegmentationPostProcessedPS.usf"), TEXT("MainPS"), SF_Pixel)

// RDG

IMPLEMENT_SHADER_TYPE(, FLivRDGScreenPassVS, TEXT("/Plugin/Liv/LivRDGScreenPassVS.usf"), TEXT("MainVS"), SF_Vertex)
IMPLEMENT_SHADER_TYPE(, FLivRDGInvertAlphaPS, TEXT("/Plugin/Liv/LivRDGInvertAlphaPS.usf"), TEXT("MainPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, FLivRDGCombineAlphaPS, TEXT("/Plugin/Liv/LivRDGCombineAlphaPS.usf"), TEXT("MainPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, FLivRDGForegroundSegmentationAndCopyPS, TEXT("/Plugin/Liv/LivRDGForegroundSegmentationAndCopyPS.usf"), TEXT("MainPS"), SF_Pixel)
IMPLEMENT_SHADER_TYPE(, FLivRDGForegroundSegmentationPPAndCopyPS, TEXT("/Plugin/Liv/LivRDGForegroundSegmentationPPAndCopyPS.usf"), TEXT("MainPS"), SF_Pixel)

//////////////////////////////////////////////////////////////////////////

void FLivInvertAlphaPixelShader::SetInputParameters(FRHICommandList& InRHICmdList, FTextureResource* InInputTexture)
{
	SetTextureParameter(InRHICmdList, 
		InRHICmdList.GetBoundPixelShader(), 
		InputTexture, 
		InputTextureSampler, 
		TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(), 
		InInputTexture->TextureRHI);
}

void FLivInvertAlphaPixelShader::SetOutputParameters(FRHICommandList& InRHICmdList, FTextureResource* InOutputTexture)
{
	SetTextureParameter(InRHICmdList, 
		InRHICmdList.GetBoundPixelShader(), 
		OutputTexture,
		OutputTextureSampler,
		TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(), 
		InOutputTexture->TextureRHI);
}

void FLivInvertAlphaPixelShader::BindParams(const FShaderParameterMap& ParameterMap)
{
	InputTexture.Bind(ParameterMap, TEXT("InputTexture"));
	InputTextureSampler.Bind(ParameterMap, TEXT("InputTextureSampler"));

	OutputTexture.Bind(ParameterMap, TEXT("OutputTexture"));
	OutputTextureSampler.Bind(ParameterMap, TEXT("OutputTextureSampler"));
}

//////////////////////////////////////////////////////////////////////////


void FLivCopyPixelShader::SetInputParameters(FRHICommandList& InRHICmdList, FTextureResource* InInputTexture)
{
	SetTextureParameter(InRHICmdList,
		InRHICmdList.GetBoundPixelShader(),
		InputTexture,
		InputTextureSampler,
		TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(),
		InInputTexture->TextureRHI);
}


void FLivCopyPixelShader::SetOutputParameters(FRHICommandList& InRHICmdList, FTextureResource* InOutputTexture)
{
	SetTextureParameter(InRHICmdList,
		InRHICmdList.GetBoundPixelShader(),
		OutputTexture,
		OutputTextureSampler,
		TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(),
		InOutputTexture->TextureRHI);
}

void FLivCopyPixelShader::BindParams(const FShaderParameterMap& ParameterMap)
{
	InputTexture.Bind(ParameterMap, TEXT("InputTexture"));
	InputTextureSampler.Bind(ParameterMap, TEXT("InputTextureSampler"));

	OutputTexture.Bind(ParameterMap, TEXT("OutputTexture"));
	OutputTextureSampler.Bind(ParameterMap, TEXT("OutputTextureSampler"));
}

//////////////////////////////////////////////////////////////////////////

void FLivCombineAlphaShader::SetInputParameters(FRHICommandList& InRHICmdList, FTextureResource* InColorTexture, FTextureResource* InAlphaTexture)
{
	SetTextureParameter(InRHICmdList,
		InRHICmdList.GetBoundPixelShader(),
		InputColorTexture,
		InputColorTextureSampler,
		TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(),
		InColorTexture->TextureRHI);

	SetTextureParameter(InRHICmdList,
		InRHICmdList.GetBoundPixelShader(),
		InputAlphaTexture,
		InputAlphaTextureSampler,
		TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(),
		InAlphaTexture->TextureRHI);
}


void FLivCombineAlphaShader::SetOutputParameters(FRHICommandList& InRHICmdList, FTextureResource* InOutputTexture)
{
	SetTextureParameter(InRHICmdList,
		InRHICmdList.GetBoundPixelShader(),
		OutputTexture,
		OutputTextureSampler,
		TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(),
		InOutputTexture->TextureRHI);
}

void FLivCombineAlphaShader::BindParams(const FShaderParameterMap& ParameterMap)
{
	InputColorTexture.Bind(ParameterMap, TEXT("InputColorTexture"));
	InputColorTextureSampler.Bind(ParameterMap, TEXT("InputColorTextureSampler"));

	InputAlphaTexture.Bind(ParameterMap, TEXT("InputAlphaTexture"));
	InputAlphaTextureSampler.Bind(ParameterMap, TEXT("InputAlphaTextureSampler"));

	OutputTexture.Bind(ParameterMap, TEXT("OutputTexture"));
	OutputTextureSampler.Bind(ParameterMap, TEXT("OutputTextureSampler"));
}

//////////////////////////////////////////////////////////////////////////

void FLivForegroundSegmentationPixelShader::SetParameters(FRHICommandList& InRHICmdList, 
	FTextureResource* InInputBackgroundTexture, 
	FTextureResource* InInputForegroundTexture,
	FTextureResource* InOutputTexture)
{
	SetTextureParameter(InRHICmdList,
		InRHICmdList.GetBoundPixelShader(),
		InputBackgroundTexture,
		InputBackgroundTextureSampler,
		TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(),
		InInputBackgroundTexture->TextureRHI);

	SetTextureParameter(InRHICmdList,
		InRHICmdList.GetBoundPixelShader(),
		InputForegroundTexture,
		InputForegroundTextureSampler,
		TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(),
		InInputForegroundTexture->TextureRHI);

	SetTextureParameter(InRHICmdList,
		InRHICmdList.GetBoundPixelShader(),
		OutputTexture,
		OutputTextureSampler,
		TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(),
		InOutputTexture->TextureRHI);
}

void FLivForegroundSegmentationPixelShader::BindParams(const FShaderParameterMap& ParameterMap)
{
	InputBackgroundTexture.Bind(ParameterMap, TEXT("InputBackgroundTexture"));
	InputBackgroundTextureSampler.Bind(ParameterMap, TEXT("InputBackgroundTextureSampler"));

	InputForegroundTexture.Bind(ParameterMap, TEXT("InputForegroundTexture"));
	InputForegroundTextureSampler.Bind(ParameterMap, TEXT("InputForegroundTextureSampler"));

	OutputTexture.Bind(ParameterMap, TEXT("OutputTexture"));
	OutputTextureSampler.Bind(ParameterMap, TEXT("OutputTextureSampler"));
}

//////////////////////////////////////////////////////////////////////////

void FLivForegroundSegmentationPostProcessedPixelShader::SetParameters(FRHICommandList& InRHICmdList, 
	FTextureResource* InInputSceneColorTexture, 
	FTextureResource* InInputBackgroundDepthTexture, 
	FTextureResource* InInputForegroundDepthTexture, 
	FTextureResource* InOutputTexture)
{
	SetTextureParameter(InRHICmdList,
		InRHICmdList.GetBoundPixelShader(),
		InputSceneColorTexture,
		InputSceneColorTextureSampler,
		TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(),
		InInputSceneColorTexture->TextureRHI);

	SetTextureParameter(InRHICmdList,
		InRHICmdList.GetBoundPixelShader(),
		InputBackgroundDepthTexture,
		InputBackgroundDepthTextureSampler,
		TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(),
		InInputBackgroundDepthTexture->TextureRHI);

	SetTextureParameter(InRHICmdList,
		InRHICmdList.GetBoundPixelShader(),
		InputForegroundDepthTexture,
		InputForegroundDepthTextureSampler,
		TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(),
		InInputForegroundDepthTexture->TextureRHI);

	SetTextureParameter(InRHICmdList,
		InRHICmdList.GetBoundPixelShader(),
		OutputTexture,
		OutputTextureSampler,
		TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI(),
		InOutputTexture->TextureRHI);
}

void FLivForegroundSegmentationPostProcessedPixelShader::BindParams(const FShaderParameterMap& ParameterMap)
{
	InputSceneColorTexture.Bind(ParameterMap, TEXT("InputSceneColorTexture"));
	InputSceneColorTextureSampler.Bind(ParameterMap, TEXT("InputSceneColorTextureSampler"));

	InputBackgroundDepthTexture.Bind(ParameterMap, TEXT("InputBackgroundDepthTexture"));
	InputBackgroundDepthTextureSampler.Bind(ParameterMap, TEXT("InputBackgroundDepthTextureSampler"));

	InputForegroundDepthTexture.Bind(ParameterMap, TEXT("InputForegroundDepthTexture"));
	InputForegroundDepthTextureSampler.Bind(ParameterMap, TEXT("InputForegroundDepthTextureSampler"));

	OutputTexture.Bind(ParameterMap, TEXT("OutputTexture"));
	OutputTextureSampler.Bind(ParameterMap, TEXT("OutputTextureSampler"));
}

//////////////////////////////////////////////////////////////////////////
