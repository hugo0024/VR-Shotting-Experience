// Copyright 2021 LIV Inc. - MIT License
#include "LivTests.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "LIV.h"
#include "Windows/HideWindowsPlatformTypes.h"

#include "LivConversions.h"

#include "EngineGlobals.h"
#include "Tests/AutomationCommon.h"

#if WITH_DEV_AUTOMATION_TESTS

static constexpr auto GAutomationFlags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ServerContext | EAutomationTestFlags::EngineFilter;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLivMatrixToUnrealMatrix, "LIV.Math.Liv Matrix to Unreal Matrix", GAutomationFlags)

/**
 * Test conversion of Liv matrix to a UE4 matrix.
 */
bool FLivMatrixToUnrealMatrix::RunTest(const FString& Parameters)
{
	const LIV_Matrix4x4 LivMatrix
	{
		0.866f, 0.5f,	0.0f, 5.0f,
		-0.5f,	0.866f, 0.0f, 2.0f,
		0.0f,	0.0f,	1.0f, 10.0f,
		0.0f,	0.0f,	0.0f, 1.0f
	};

	FMatrix UnrealMatrix = FMatrix::Identity;

	// Row 0
	UnrealMatrix.M[0][0] = 1.0f; 
	UnrealMatrix.M[0][1] = 0.0f;
	UnrealMatrix.M[0][2] = 0.0f;
	UnrealMatrix.M[0][3] = 0.0f;

	// Row 1
	UnrealMatrix.M[1][0] = 0.0f;
	UnrealMatrix.M[1][1] = 0.866f;
	UnrealMatrix.M[1][2] = -0.5f;
	UnrealMatrix.M[1][3] = 0.0f;

	// Row 2
	UnrealMatrix.M[2][0] = 0.0f;
	UnrealMatrix.M[2][1] = 0.5f;
	UnrealMatrix.M[2][2] = 0.866f;
	UnrealMatrix.M[2][3] = 0.0f;

	// Row 3
	UnrealMatrix.M[3][0] = 1000.0f;
	UnrealMatrix.M[3][1] = 500.0f;
	UnrealMatrix.M[3][2] = 200.0f;
	UnrealMatrix.M[3][3] = 1.0f;

	// Convert from Liv space (Unity) to UE4
	const FMatrix ConvertedMatrix = Convert<LIV_Matrix4x4, FMatrix>(LivMatrix);

	// Test equality of converted matrix - element by element
	for (int32 Row{ 0 }; Row < 4; Row++)
	{
		for (int32 Col{ 0 }; Col < 4; Col++)
		{
			const FString What = FString::Printf(TEXT("[%d,%d]"), Row, Col);
			TestEqual<float>(What, ConvertedMatrix.M[Row][Col], UnrealMatrix.M[Row][Col]);
		}
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLivPositionToUnrealPosition, "LIV.Math.Liv Position to Unreal Position", GAutomationFlags)

/**
 * Test conversion of a Liv space location to a UE4 space location.
 */
bool FLivPositionToUnrealPosition::RunTest(const FString& Parameters)
{
	const LIV_Vector3 LivLocation{ 3.0f, 2.0f, 9.0f };
	const FVector UnrealLocation(900.0f, 300.0f, 200.0f);

	const FVector ConvertedLocation = ConvertPosition<LIV_Vector3, FVector>(LivLocation);

	TestEqual<float>(TEXT("Pos X"), ConvertedLocation.X, UnrealLocation.X);
	TestEqual<float>(TEXT("Pos Y"), ConvertedLocation.Y, UnrealLocation.Y);
	TestEqual<float>(TEXT("Pos Z"), ConvertedLocation.Z, UnrealLocation.Z);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFieldOfViewConversions, "LIV.Math.FOV Conversions", GAutomationFlags)

/**
 * Test conversions between vertical field of view and horizontal field of view.
 */
bool FFieldOfViewConversions::RunTest(const FString& Parameters)
{
	// 16:9 aspect ratio
	const float Width = 16.0f;
	const float Height = 9.0f;
	const float AspectRatio = Width / Height;
	const float VerticalFOV = 59.0f;
	const float HorizontalFOV = 90.0f;

	//////////////////////////////////////////////////////////////////////////

	const float ActualHorizontalFOVFromRatio = FMath::RoundToFloat(ConvertVerticalFOVToHorizontalFOV(VerticalFOV, AspectRatio));
	TestEqual<float>(TEXT("Horizontal FOV from Aspect Ratio"), ActualHorizontalFOVFromRatio, HorizontalFOV);

	const float ActualHorizontalFOVFromWidthHeight = FMath::RoundToFloat(ConvertVerticalFOVToHorizontalFOV(VerticalFOV, Width, Height));
	TestEqual<float>(TEXT("Horizontal FOV from Width and Height"), ActualHorizontalFOVFromWidthHeight, HorizontalFOV);

	//////////////////////////////////////////////////////////////////////////

	const float ActualVerticalFOVFromRatio = FMath::RoundToFloat(ConvertHorizontalFOVToVerticalFOV(HorizontalFOV, AspectRatio));
	TestEqual<float>(TEXT("Vertical FOV from Aspect Ratio"), ActualVerticalFOVFromRatio, VerticalFOV);

	const float ActualVerticalFOVFromWidthHeight = FMath::RoundToFloat(ConvertHorizontalFOVToVerticalFOV(HorizontalFOV, Width, Height));
	TestEqual<float>(TEXT("Vertical FOV from Width and Height"), ActualVerticalFOVFromWidthHeight, VerticalFOV);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
