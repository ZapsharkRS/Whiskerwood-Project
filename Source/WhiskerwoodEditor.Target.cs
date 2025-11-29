// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class WhiskerwoodEditorTarget : TargetRules
{
	public WhiskerwoodEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		// Use the latest UE5-style defaults
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;

		// Your game module
		ExtraModuleNames.Add("Whiskerwood");

		// ---------- FAST-BUILD SETTINGS ----------

		// Keep using Unity builds (compile multiple .cpps together for speed)
		// Good for overall speed, can make small changes recompile more stuff,
		// but usually a win while the project is small/medium.
		bUseUnityBuild = true;
		bUseAdaptiveUnityBuild = true;

		// PCHs = fewer headers compiled over and over
		bUsePCHFiles = true;
		bUseSharedPCHs = true;

		// Just making these explicit – they’re default for Editor targets
		bCompileAgainstEngine = true;
		bCompileAgainstCoreUObject = true;

		// If you ever want super-aggressive debug builds (faster linker),
		// you could toggle these depending on configuration, but I’d leave
		// them at defaults unless you know you need to change them:
		//
		// bIsBuildingConsoleApplication = false;
		// bUseIncrementalLinking = true;  // typically default in Dev Editor
	}
}