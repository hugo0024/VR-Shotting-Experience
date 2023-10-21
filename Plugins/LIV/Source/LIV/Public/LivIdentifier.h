// Copyright 2021 LIV Inc. - MIT License
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LivIdentifier.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct LIV_API FLivIdentifier
{
	GENERATED_BODY()

	// Name of engine
	UPROPERTY(BlueprintReadWrite)
		FString EngineName;

	// Version of engine
	UPROPERTY(BlueprintReadWrite)
		FString EngineVersion;

	// Liv plugin version
	UPROPERTY(BlueprintReadWrite)
		FString ClientVersion;

	// Liv native SDK version
	UPROPERTY(BlueprintReadWrite)
		FString NativeClientVersion;
	
};
