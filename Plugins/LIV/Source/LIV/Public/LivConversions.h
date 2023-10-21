// Copyright 2021 LIV Inc. - MIT License
#pragma once

#include "Windows/AllowWindowsPlatformTypes.h"
#include <dxgi.h>
#include "LIV_BridgeDatastruct.h"
#include "Windows/HideWindowsPlatformTypes.h"

#include "Kismet/KismetMathLibrary.h"


/**
 * Copied from D3D11Viewport.h:45
 * (Not sure I want this plugin to list D3D11RHI as a module dependency).
 */
DXGI_FORMAT GetRenderTargetFormat(const EPixelFormat PixelFormat);

template<typename FromType, typename ToType>
inline ToType Convert(const FromType& From) {}

/**
 * Convert Unity/Liv space matrix to UE4 space matrix
 */
template<>
inline FMatrix Convert<LIV_Matrix4x4, FMatrix>(const LIV_Matrix4x4& LivMatrix)
{
	FMatrix M(EForceInit::ForceInitToZero);

	M.M[0][0] = LivMatrix.m22;
	M.M[0][1] = LivMatrix.m02;
	M.M[0][2] = LivMatrix.m12;
	M.M[0][3] = LivMatrix.m30;

	M.M[1][0] = LivMatrix.m20;
	M.M[1][1] = LivMatrix.m00;
	M.M[1][2] = LivMatrix.m10;
	M.M[1][3] = LivMatrix.m31;

	M.M[2][0] = LivMatrix.m21;
	M.M[2][1] = LivMatrix.m01;
	M.M[2][2] = LivMatrix.m11;
	M.M[2][3] = LivMatrix.m32;

	M.M[3][0] = LivMatrix.m23 * 100.0f;
	M.M[3][1] = LivMatrix.m03 * 100.0f;
	M.M[3][2] = LivMatrix.m13 * 100.0f;
	M.M[3][3] = LivMatrix.m33;

	return M;
}

/**
 * Convert Unity/Liv space quaternion to UE4 space quaternion
 */
template<>
inline FQuat Convert<LIV_Quaternion, FQuat>(const LIV_Quaternion& LivQuaterion)
{
	return FQuat(LivQuaterion.z, LivQuaterion.x, LivQuaterion.y, LivQuaterion.w);
}

template<typename FromType, typename ToType>
inline ToType ConvertPosition(const FromType& From) {}

/**
 * Convert Unity/Liv space position to UE4 space position
 * UE4				Unity
 * X		=		Z * 100
 * Y		=		X * 100
 * Z		=		Y * 100
 */
template<>
inline FVector ConvertPosition<LIV_Vector3, FVector>(const LIV_Vector3& LivVector)
{
	const float Scale = 100.0f;
	return FVector(LivVector.z * Scale, LivVector.x * Scale, LivVector.y * Scale);
}

static inline float AspectRatio(const float Width, const float Height)
{
	return Width / Height;
}

/**
 * Convert Vertical Field of View (FOV) to Horizontal FOV given an aspect ratio.
 */
static inline float ConvertVerticalFOVToHorizontalFOV(const float VerticalFOV, const float AspectRatio)
{
	return 2.0f * UKismetMathLibrary::DegAtan( AspectRatio * UKismetMathLibrary::DegTan(VerticalFOV * 0.5f) );
}

/**
 * Convert Vertical Field of View (FOV) to Horizontal FOV given a width and height.
 */
static inline float ConvertVerticalFOVToHorizontalFOV(const float VerticalFOV, const float Width, const float Height)
{
	return ConvertVerticalFOVToHorizontalFOV(VerticalFOV, Width / Height);
}

/**
 * Convert Horizontal Field of View (FOV) to Vertical FOV given an aspect ratio.
 */
static inline float ConvertHorizontalFOVToVerticalFOV(const float HorizontalFOV, const float AspectRatio)
{
	return 2.0f * UKismetMathLibrary::DegAtan( (1.0f / AspectRatio) * UKismetMathLibrary::DegTan(HorizontalFOV * 0.5f));
}

/**
 * Convert Horizontal Field of View (FOV) to Vertical FOV given a width and height.
 */
static inline float ConvertHorizontalFOVToVerticalFOV(const float HorizontalFOV, const float Width, const float Height)
{
	return 2.0f * UKismetMathLibrary::DegAtan( (Height / Width) * UKismetMathLibrary::DegTan(HorizontalFOV * 0.5f));
}