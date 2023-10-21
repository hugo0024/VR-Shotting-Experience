// Copyright 2021 LIV Inc. - MIT License
using UnrealBuildTool;

public class LIVEditor : ModuleRules
{
    public LIVEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Analytics",
                "ComponentVisualizers",
                "CoreUObject",
                "Engine",
                "InputCore",
                "HTTP",
                "LIV",
                "Projects",
                "Slate",
                "SlateCore",
                "RenderCore",
                "RHI",
                "ToolMenus",
                "UnrealEd",
                "WorkspaceMenuStructure"
            }
        );
    }
}
