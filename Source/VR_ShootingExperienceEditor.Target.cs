// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class VR_ShootingExperienceEditorTarget : TargetRules
{
	public VR_ShootingExperienceEditorTarget(TargetInfo Target) : base(Target)
	{
        DefaultBuildSettings = BuildSettingsVersion.V2;

        Type = TargetType.Editor;
        ExtraModuleNames.AddRange(new string[] { "VR_ShootingExperience" });
		//bUseUnityBuild = false;
		//bUsePCHFiles = false;
    }

	//
	// TargetRules interface.
	//

/*	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.AddRange( new string[] { "VR_ShootingExperience" } );
	}*/
}
