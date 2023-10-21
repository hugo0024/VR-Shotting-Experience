// Copyright 2021 LIV Inc. - MIT License
#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "RenderTargetPool.h"
#include "RHIDefinitions.h"
#include "Shader.h"
#include "ShaderParameters.h"
#include "ShaderParameterStruct.h"
#include "ShaderParameterMacros.h"
#include "TextureResource.h"

/**
 * Simple vertex shader for alpha inversion of render target.
 * Does not require a vertex buffer as the vertices are encoded in the shader itself.
 */
class FLivSingleTextureVertexShader : public FGlobalShader
{
	DECLARE_EXPORTED_SHADER_TYPE(FLivSingleTextureVertexShader, Global, LIVRENDERING_API);
public:

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::ES3_1);
	}

	static void ModifyCompilationEnvironment(const FShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}

	FLivSingleTextureVertexShader() {}

	FLivSingleTextureVertexShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
	}
};

/**
 * Pixel shader for alpha inversion of render target.
 * Simply reads from one render target and writes to another with the alpha inverted.
 */
class FLivInvertAlphaPixelShader : public FGlobalShader
{
	DECLARE_EXPORTED_SHADER_TYPE(FLivInvertAlphaPixelShader, Global, LIVRENDERING_API);

public:

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::ES3_1);
	}

	static void ModifyCompilationEnvironment(const FShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}

	FLivInvertAlphaPixelShader() {}

	FLivInvertAlphaPixelShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		BindParams(Initializer.ParameterMap);
	}

	LIVRENDERING_API void SetInputParameters(FRHICommandList& InRHICmdList, FTextureResource* InInputTexture);
	LIVRENDERING_API void SetOutputParameters(FRHICommandList& InRHICmdList, FTextureResource* InOutputTexture);

	void BindParams(const FShaderParameterMap& ParameterMap);

protected:

	LAYOUT_FIELD(FShaderResourceParameter, InputTexture);
	LAYOUT_FIELD(FShaderResourceParameter, InputTextureSampler);

	LAYOUT_FIELD(FShaderResourceParameter, OutputTexture);
	LAYOUT_FIELD(FShaderResourceParameter, OutputTextureSampler);

};


/**
 * Pixel shader to copy from one render target to another.
 */
class FLivCopyPixelShader : public FGlobalShader
{
	DECLARE_EXPORTED_SHADER_TYPE(FLivCopyPixelShader, Global, LIVRENDERING_API);

public:

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::ES3_1);
	}

	static void ModifyCompilationEnvironment(const FShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}

	FLivCopyPixelShader() {}

	FLivCopyPixelShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		BindParams(Initializer.ParameterMap);
	}

	LIVRENDERING_API void SetInputParameters(FRHICommandList& InRHICmdList, FTextureResource* InInputTexture);
	LIVRENDERING_API void SetOutputParameters(FRHICommandList& InRHICmdList, FTextureResource* InOutputTexture);

	void BindParams(const FShaderParameterMap& ParameterMap);

protected:

	LAYOUT_FIELD(FShaderResourceParameter, InputTexture);
	LAYOUT_FIELD(FShaderResourceParameter, InputTextureSampler);

	LAYOUT_FIELD(FShaderResourceParameter, OutputTexture);
	LAYOUT_FIELD(FShaderResourceParameter, OutputTextureSampler);

};


class FLivCombineAlphaShader : public FGlobalShader
{
	DECLARE_EXPORTED_SHADER_TYPE(FLivCombineAlphaShader, Global, LIVRENDERING_API);

public:

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::ES3_1);
	}

	static void ModifyCompilationEnvironment(const FShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}

	FLivCombineAlphaShader() {}

	FLivCombineAlphaShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		BindParams(Initializer.ParameterMap);
	}

	LIVRENDERING_API void SetInputParameters(FRHICommandList& InRHICmdList, FTextureResource* InColorTexture, FTextureResource* InAlphaTexture);
	LIVRENDERING_API void SetOutputParameters(FRHICommandList& InRHICmdList, FTextureResource* InOutputTexture);

	void BindParams(const FShaderParameterMap& ParameterMap);

protected:

	LAYOUT_FIELD(FShaderResourceParameter, InputColorTexture);
	LAYOUT_FIELD(FShaderResourceParameter, InputColorTextureSampler);

	LAYOUT_FIELD(FShaderResourceParameter, InputAlphaTexture);
	LAYOUT_FIELD(FShaderResourceParameter, InputAlphaTextureSampler);

	LAYOUT_FIELD(FShaderResourceParameter, OutputTexture);
	LAYOUT_FIELD(FShaderResourceParameter, OutputTextureSampler);

};


/**
 * Pixel shader for foreground segmentation when not using the global clip plane.
 */
class FLivForegroundSegmentationPixelShader : public FGlobalShader
{
	DECLARE_EXPORTED_SHADER_TYPE(FLivForegroundSegmentationPixelShader, Global, LIVRENDERING_API);

public:

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::ES3_1);
	}

	static void ModifyCompilationEnvironment(const FShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}

	FLivForegroundSegmentationPixelShader() {}

	FLivForegroundSegmentationPixelShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		BindParams(Initializer.ParameterMap);
	}

	LIVRENDERING_API void SetParameters(FRHICommandList& InRHICmdList, 
		FTextureResource* InInputBackgroundTexture, 
		FTextureResource* InInputForegroundTexture,
		FTextureResource* InOutputTexture);

	void BindParams(const FShaderParameterMap& ParameterMap);

protected:

	LAYOUT_FIELD(FShaderResourceParameter, InputForegroundTexture);
	LAYOUT_FIELD(FShaderResourceParameter, InputForegroundTextureSampler);

	LAYOUT_FIELD(FShaderResourceParameter, InputBackgroundTexture);
	LAYOUT_FIELD(FShaderResourceParameter, InputBackgroundTextureSampler);

	LAYOUT_FIELD(FShaderResourceParameter, OutputTexture);
	LAYOUT_FIELD(FShaderResourceParameter, OutputTextureSampler);

};


/**
 * Pixel shader for foreground segmentation when not using the global clip plane and using post processing.
 */
class FLivForegroundSegmentationPostProcessedPixelShader : public FGlobalShader
{
	DECLARE_EXPORTED_SHADER_TYPE(FLivForegroundSegmentationPostProcessedPixelShader, Global, LIVRENDERING_API);

public:

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::ES3_1);
	}

	static void ModifyCompilationEnvironment(const FShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}

	FLivForegroundSegmentationPostProcessedPixelShader() {}

	FLivForegroundSegmentationPostProcessedPixelShader(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{
		BindParams(Initializer.ParameterMap);
	}

	LIVRENDERING_API void SetParameters(FRHICommandList& InRHICmdList,
		FTextureResource* InInputSceneColorTexture,
		FTextureResource* InInputBackgroundDepthTexture,
		FTextureResource* InInputForegroundDepthTexture,
		FTextureResource* InOutputTexture);

	void BindParams(const FShaderParameterMap& ParameterMap);

protected:

	LAYOUT_FIELD(FShaderResourceParameter, InputSceneColorTexture);
	LAYOUT_FIELD(FShaderResourceParameter, InputSceneColorTextureSampler);

	LAYOUT_FIELD(FShaderResourceParameter, InputBackgroundDepthTexture);
	LAYOUT_FIELD(FShaderResourceParameter, InputBackgroundDepthTextureSampler);

	LAYOUT_FIELD(FShaderResourceParameter, InputForegroundDepthTexture);
	LAYOUT_FIELD(FShaderResourceParameter, InputForegroundDepthTextureSampler);

	LAYOUT_FIELD(FShaderResourceParameter, OutputTexture);
	LAYOUT_FIELD(FShaderResourceParameter, OutputTextureSampler);

};

/**
 * RDG Shaders
 */

BEGIN_SHADER_PARAMETER_STRUCT(FLivSubmitParameters, LIVRENDERING_API)
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, BackgroundTexture)
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ForegroundTexture)
END_SHADER_PARAMETER_STRUCT()

class FLivRDGScreenPassVS : public FGlobalShader
{
public:

	DECLARE_EXPORTED_SHADER_TYPE(FLivRDGScreenPassVS, Global, LIVRENDERING_API);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters&)
	{
		return true;
	}

	FLivRDGScreenPassVS() = default;
	FLivRDGScreenPassVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FGlobalShader(Initializer)
	{}
};

/**
* Pixel shader for alpha inversion of render target.
* Simply reads from one render target and writes to another with the alpha inverted.
*/
class FLivRDGInvertAlphaPS : public FGlobalShader
{
public:

	DECLARE_EXPORTED_SHADER_TYPE(FLivRDGInvertAlphaPS, Global, LIVRENDERING_API);

	SHADER_USE_PARAMETER_STRUCT(FLivRDGInvertAlphaPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_TEXTURE(Texture2D, InputTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, InputSampler)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::ES3_1);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}
};

/**
 * Combines RGB color of one input and alpha of another.
 */
class FLivRDGCombineAlphaPS : public FGlobalShader
{
public:

	DECLARE_EXPORTED_SHADER_TYPE(FLivRDGCombineAlphaPS, Global, LIVRENDERING_API);

	SHADER_USE_PARAMETER_STRUCT(FLivRDGCombineAlphaPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_TEXTURE(Texture2D, InputColorTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, InputColorSampler)
		SHADER_PARAMETER_TEXTURE(Texture2D, InputAlphaTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, InputAlphaSampler)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

		static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::ES3_1);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}
};

/**
 * Does foreground segmentation for first bound render target and
 * a copy for the second render target.
 */
class FLivRDGForegroundSegmentationAndCopyPS : public FGlobalShader
{
public:

	DECLARE_EXPORTED_SHADER_TYPE(FLivRDGForegroundSegmentationAndCopyPS, Global, LIVRENDERING_API);

	SHADER_USE_PARAMETER_STRUCT(FLivRDGForegroundSegmentationAndCopyPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_TEXTURE(Texture2D, InputBackgroundTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, InputBackgroundSampler)
		SHADER_PARAMETER_TEXTURE(Texture2D, InputForegroundTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, InputForegroundSampler)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

		static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::ES3_1);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}
};

/**
 * Does foreground segmentation (post processed) for first bound render target and
 * a copy for the second render target.
 */
class FLivRDGForegroundSegmentationPPAndCopyPS : public FGlobalShader
{
public:

	DECLARE_EXPORTED_SHADER_TYPE(FLivRDGForegroundSegmentationPPAndCopyPS, Global, LIVRENDERING_API);

	SHADER_USE_PARAMETER_STRUCT(FLivRDGForegroundSegmentationPPAndCopyPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_TEXTURE(Texture2D, InputBackgroundTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, InputBackgroundSampler)
		SHADER_PARAMETER_TEXTURE(Texture2D, InputBackgroundDepthTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, InputBackgroundDepthSampler)
		SHADER_PARAMETER_TEXTURE(Texture2D, InputForegroundDepthTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, InputForegroundDepthSampler)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

		static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::ES3_1);
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}
};

