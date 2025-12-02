// WhiskerwoodEditorUserSettings.cpp

#include "WhiskerwoodEditorUserSettings.h"

#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "HAL/PlatformFile.h"

// If you want to use the existing mods dir helper:
#include "WhiskerwoodFileIOLibrary.h"

UWhiskerwoodEditorUserSettings::UWhiskerwoodEditorUserSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// --------------------------------------------------------------------
	// Establish sensible defaults for a first-time user.
	// These are only used when the corresponding config entry is empty.
	// --------------------------------------------------------------------

	// Project Directory → current project root.
	if (ProjectDirectory.IsEmpty())
	{
		ProjectDirectory = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
		FPaths::NormalizeDirectoryName(ProjectDirectory);
	}

	// Mods Directory → prefer helper (LOCALAPPDATA/Whiskerwood/Saved/mods)
	if (ModsDirectory.IsEmpty())
	{
		FString AutoModsDir = UWhiskerwoodFileIOLibrary::GetWhiskerwoodModsDirectory();
		FPaths::NormalizeDirectoryName(AutoModsDir);
		ModsDirectory = AutoModsDir;
	}

	// AppData Directory → derive from ModsDirectory:
	// ModsDirectory = <AppData>/Whiskerwood/Saved/mods
	// AppDataDirectory = <AppData>/Whiskerwood
	if (AppDataDirectory.IsEmpty())
	{
		FString SavedDir = FPaths::GetPath(ModsDirectory);   // .../Whiskerwood/Saved
		FString RootDir  = FPaths::GetPath(SavedDir);        // .../Whiskerwood
		FPaths::NormalizeDirectoryName(RootDir);
		AppDataDirectory = RootDir;
	}

	// Base Logs Directory → <AppData>/Whiskerwood/Saved/Logs
	if (BaseLogsDirectory.IsEmpty())
	{
		FString LogsDir = FPaths::Combine(AppDataDirectory, TEXT("Saved"), TEXT("Logs"));
		FPaths::NormalizeDirectoryName(LogsDir);
		BaseLogsDirectory = LogsDir;
	}

	// Temporary Deploy Directory → <AppData>/Whiskerwood/TempWorkshop
	if (TempDeployDirectory.IsEmpty())
	{
		FString TempDir = FPaths::Combine(AppDataDirectory, TEXT("TempWorkshop"));
		FPaths::NormalizeDirectoryName(TempDir);
		TempDeployDirectory = TempDir;
	}

	// Pak Directory → leave empty by default; tools can search from ProjectDirectory.
	// You can later auto-detect this if you want, e.g. scanning for pakchunk*.pak.
	if (!PakDirectory.IsEmpty())
	{
		FPaths::NormalizeDirectoryName(PakDirectory);
	}

	// Platform Name → default to "Windows" for now.
	if (PlatformName.IsEmpty())
	{
		PlatformName = TEXT("Windows");
	}

	// Steam App ID → default to Whiskerwood's ID.
	if (SteamAppID <= 0)
	{
		SteamAppID = 2489330;
	}

	// Normalise LastCopiedPakPath if present.
	if (!LastCopiedPakPath.IsEmpty())
	{
		FPaths::NormalizeFilename(LastCopiedPakPath);
	}
}

void UWhiskerwoodEditorUserSettings::SaveSettings()
{
	// This will write to EditorPerProjectUserSettings.ini for this project.
	SaveConfig();
}

UWhiskerwoodEditorUserSettings* UWhiskerwoodEditorUserSettings::GetSettings()
{
	return GetMutableDefault<UWhiskerwoodEditorUserSettings>();
}
