// Copyright 2021 LIV Inc. - MIT License
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class LIV : ModuleRules
    {
        public LIV(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            PrivateIncludePaths.AddRange(new string[]
            {
                Path.Combine(ModuleDirectory, "Private"),
                Path.Combine(ModuleDirectory, "Private", "Test"),
                EngineDirectory + "/Source/Runtime/Renderer/Private",
                EngineDirectory + "/Source/Runtime/Renderer/Private/PostProcess"
            });

            PublicIncludePaths.AddRange(new string[]
            {
                Path.Combine(ModuleDirectory, "Public"),
                Path.Combine(ModuleDirectory, "Public", "Test")
            });

            PublicDependencyModuleNames.AddRange(new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "EngineSettings",
                "HeadMountedDisplay",
                "InputCore",
                "Json",
                "JsonUtilities",
                "LivSDK",
                "LivRendering",
                "Projects",
                "RenderCore",
                "Renderer",
                "RHI"
            });
            
            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                switch (Target.Type)
                {
                    case TargetType.Client:
                    case TargetType.Server:
                    case TargetType.Game:
                        string LivIdentifierDestinationPath = Path.Combine("$(TargetOutputDir)", "app.livsdk");
                        string LivIdentifierSourcePath =
                            Path.Combine(ModuleDirectory, "..", "..", "Resources", "app.livsdk");

                        if (System.IO.File.Exists(LivIdentifierSourcePath))
                        {
                            RuntimeDependencies.Add(LivIdentifierDestinationPath, LivIdentifierSourcePath,
                                StagedFileType.SystemNonUFS);
                        }
                        else
                        {
                            throw new FileNotFoundException(
                                "Missing 'app.livsdk' required for games packaged with LIV.",
                                "app.livsdk");
                        }

                        break;
                }
            }
        }
    }
}
