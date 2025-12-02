// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WhiskerwoodModTools : ModuleRules
{
	public WhiskerwoodModTools(ReadOnlyTargetRules Target) : base(Target)
	{
		
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"UMG",
			"Slate",
			"SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Slate",
			"SlateCore",
			"UnrealEd",
			"AssetRegistry",
			"Projects",
			"Blutility",
			"UMGEditor",
			"Json",
			"JsonUtilities",
			"UnrealEd",
			"Blutility",
			"UMGEditor",
			"EditorSubsystem",
			"LevelEditor",
			"ToolMenus"
			
		});
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
		if (Target.bBuildEditor)
		{

		}
	}
}
