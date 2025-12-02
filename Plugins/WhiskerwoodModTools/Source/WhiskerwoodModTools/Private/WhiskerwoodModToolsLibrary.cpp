#include "WhiskerwoodModToolsLibrary.h"

#include "WhiskerwoodModListItem.h"
#include "PAL_WhiskerwoodModSettings.h"
#include "WhiskerwoodFileIOLibrary.h"
#include "WhiskerwoodEditorUserSettings.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Modules/ModuleManager.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformFileManager.h"
#include "HAL/PlatformProcess.h"

// ============================================================================
// GetAllWhiskerwoodModSettings
// ============================================================================

void UWhiskerwoodModToolsLibrary::GetAllWhiskerwoodModSettings(
	TArray<UPAL_WhiskerwoodModSettings*>& OutModSettings)
{
	OutModSettings.Reset();


	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.bRecursiveClasses = true;
	Filter.bIncludeOnlyOnDiskAssets = false;

	// UE5.6+: use ClassPaths instead of ClassNames
	Filter.ClassPaths.Add(UPAL_WhiskerwoodModSettings::StaticClass()->GetClassPathName());

	TArray<FAssetData> AssetDataList;
	AssetRegistry.GetAssets(Filter, AssetDataList);

	for (const FAssetData& AssetData : AssetDataList)
	{
		if (UObject* LoadedObj = AssetData.GetAsset())
		{
			if (UPAL_WhiskerwoodModSettings* ModSettings =
				    Cast<UPAL_WhiskerwoodModSettings>(LoadedObj))
			{
				OutModSettings.Add(ModSettings);
			}
		}
	}

}

FString UWhiskerwoodModToolsLibrary::ResolveProjectDirectory()
{
	const UWhiskerwoodEditorUserSettings* Settings =
		GetDefault<UWhiskerwoodEditorUserSettings>();

	if (Settings && !Settings->ProjectDirectory.IsEmpty())
	{
		FString Candidate = FPaths::ConvertRelativePathToFull(Settings->ProjectDirectory);
		FPaths::NormalizeDirectoryName(Candidate);
		return Candidate;
	}

	FString ProjectDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	FPaths::NormalizeDirectoryName(ProjectDir);
	return ProjectDir;
}

FString UWhiskerwoodModToolsLibrary::ResolveModsDirectory()
{
	const UWhiskerwoodEditorUserSettings* Settings =
		GetDefault<UWhiskerwoodEditorUserSettings>();

	// 1) User override
	if (Settings && !Settings->ModsDirectory.IsEmpty())
	{
		FString Dir = Settings->ModsDirectory;
		FPaths::NormalizeDirectoryName(Dir);
		return Dir;
	}

	// 2) Auto from helper (LOCALAPPDATA/Whiskerwood/Saved/mods)
	FString AutoModsDir = UWhiskerwoodFileIOLibrary::GetWhiskerwoodModsDirectory();
	FPaths::NormalizeDirectoryName(AutoModsDir);
	return AutoModsDir;
}


FString UWhiskerwoodModToolsLibrary::ResolveAppDataDirectory()
{
	const UWhiskerwoodEditorUserSettings* Settings =
		GetDefault<UWhiskerwoodEditorUserSettings>();

	// 1) Explicit override
	if (Settings && !Settings->AppDataDirectory.IsEmpty())
	{
		FString Dir = Settings->AppDataDirectory;
		FPaths::NormalizeDirectoryName(Dir);
		return Dir;
	}

	// 2) Derive from ModsDirectory
	const FString ModsDir = ResolveModsDirectory();

	// ModsDir = <AppData>/Whiskerwood/Saved/mods
	FString SavedDir = FPaths::GetPath(ModsDir);   // .../Whiskerwood/Saved
	FString RootDir  = FPaths::GetPath(SavedDir);  // .../Whiskerwood

	FPaths::NormalizeDirectoryName(RootDir);
	return RootDir;
}


FString UWhiskerwoodModToolsLibrary::ResolveBaseLogsDirectory()
{
	const UWhiskerwoodEditorUserSettings* Settings =
		GetDefault<UWhiskerwoodEditorUserSettings>();

	// 1) User override
	if (Settings && !Settings->BaseLogsDirectory.IsEmpty())
	{
		FString Dir = Settings->BaseLogsDirectory;
		FPaths::NormalizeDirectoryName(Dir);
		return Dir;
	}

	// 2) AppData/Saved/Logs
	const FString AppData = ResolveAppDataDirectory();
	FString LogsDir = FPaths::Combine(AppData, TEXT("Saved"), TEXT("Logs"));
	FPaths::NormalizeDirectoryName(LogsDir);
	return LogsDir;
}


FString UWhiskerwoodModToolsLibrary::ResolveTempDeployDirectory()
{
	const UWhiskerwoodEditorUserSettings* Settings =
		GetDefault<UWhiskerwoodEditorUserSettings>();

	// 1) User override
	if (Settings && !Settings->TempDeployDirectory.IsEmpty())
	{
		FString Dir = Settings->TempDeployDirectory;
		FPaths::NormalizeDirectoryName(Dir);
		return Dir;
	}

	// 2) AppData/TempWorkshop
	const FString AppData = ResolveAppDataDirectory();
	FString TempDir = FPaths::Combine(AppData, TEXT("TempWorkshop"));
	FPaths::NormalizeDirectoryName(TempDir);
	return TempDir;
}


FString UWhiskerwoodModToolsLibrary::ResolvePakDirectory()
{
	const UWhiskerwoodEditorUserSettings* Settings =
		GetDefault<UWhiskerwoodEditorUserSettings>();

	// 1) User override
	if (Settings && !Settings->PakDirectory.IsEmpty())
	{
		FString Dir = Settings->PakDirectory;
		FPaths::NormalizeDirectoryName(Dir);
		return Dir;
	}

	// 2) Try to auto-detect by scanning for any pakchunk*.pak
	const FString ProjectDir = ResolveProjectDirectory();

	TArray<FString> FoundPaks;
	IFileManager::Get().FindFilesRecursive(
		FoundPaks,
		*ProjectDir,
		TEXT("pakchunk*-*.pak"),
		true,   // files
		false   // directories
	);

	if (FoundPaks.Num() > 0)
	{
		// Take the directory of the first result as heuristic
		const FString FirstPakPath = FoundPaks[0];
		FString PakDir = FPaths::GetPath(FirstPakPath);
		FPaths::NormalizeDirectoryName(PakDir);
		return PakDir;
	}

	// 3) Give up: no pak dir known yet
	return FString();
}


FString UWhiskerwoodModToolsLibrary::ResolveUATPlatformName()
{
	const UWhiskerwoodEditorUserSettings* Settings =
		GetDefault<UWhiskerwoodEditorUserSettings>();

	FString HumanPlatform = TEXT("Windows");
	if (Settings && !Settings->PlatformName.IsEmpty())
	{
		HumanPlatform = Settings->PlatformName;
	}

	// Normalize to lower for comparison
	const FString Lower = HumanPlatform.ToLower();

	if (Lower.Contains(TEXT("win")))
	{
		return TEXT("Win64");
	}
	else if (Lower.Contains(TEXT("linux")))
	{
		return TEXT("Linux");
	}
	else if (Lower.Contains(TEXT("mac")))
	{
		// You can tweak this if you ever target Mac
		return TEXT("Mac");
	}

	// Fallback: assume Windows
	return TEXT("Win64");
}


bool UWhiskerwoodModToolsLibrary::IsNonEmptyPath(const FString& Path)
{
	return !Path.IsEmpty() && Path.Len() > 2;
}

bool UWhiskerwoodModToolsLibrary::DoesDirectoryExist(const FString& DirPath)
{
	if (DirPath.IsEmpty())
	{
		return false;
	}

	IPlatformFile& PF = FPlatformFileManager::Get().GetPlatformFile();
	return PF.DirectoryExists(*DirPath);
}

bool UWhiskerwoodModToolsLibrary::DoesFileExist(const FString& FilePath)
{
	if (FilePath.IsEmpty())
	{
		return false;
	}

	IPlatformFile& PF = FPlatformFileManager::Get().GetPlatformFile();
	return PF.FileExists(*FilePath);
}







// ============================================================================
// GetAllModListItems
// ============================================================================

void UWhiskerwoodModToolsLibrary::GetAllModListItems(
	TArray<UWhiskerwoodModListItem*>& OutItems)
{
	OutItems.Reset();


	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.bRecursiveClasses = true;
	Filter.bIncludeOnlyOnDiskAssets = false;
	Filter.ClassPaths.Add(UPAL_WhiskerwoodModSettings::StaticClass()->GetClassPathName());

	TArray<FAssetData> AssetDataList;
	AssetRegistry.GetAssets(Filter, AssetDataList);

	for (const FAssetData& AssetData : AssetDataList)
	{
		UObject* LoadedObj = AssetData.GetAsset();
		if (!LoadedObj)
		{
			continue;
		}

		UPAL_WhiskerwoodModSettings* ModSettings =
			Cast<UPAL_WhiskerwoodModSettings>(LoadedObj);

		if (!ModSettings)
		{
			continue;
		}

		UWhiskerwoodModListItem* Item = NewObject<UWhiskerwoodModListItem>();
		Item->ModSettings = ModSettings;


		Item->ModName = ModSettings->ModName.IsEmpty()
			? ModSettings->GetName()
			: ModSettings->ModName;
		Item->ChunkId = ModSettings->GetChunkId();


		Item->ModDirectoryPath = GetModDirectoryPath(ModSettings);

		OutItems.Add(Item);
	}

}

// ============================================================================
// GetModDirectoryPath
// ============================================================================

FString UWhiskerwoodModToolsLibrary::GetModDirectoryPath(
	const UPAL_WhiskerwoodModSettings* ModSettings)
{
	if (!ModSettings)
	{
		return TEXT("");
	}

	const FString BaseModsDir = ResolveModsDirectory();

	FString DirName;

	DirName = ModSettings->ModDirName;


	if (DirName.IsEmpty())
	{
		// Fallback: use asset name
		DirName = ModSettings->GetName();
	}

	FString FullPath = FPaths::Combine(BaseModsDir, DirName);
	FPaths::NormalizeDirectoryName(FullPath);

	return FullPath;
}

// ============================================================================
// WriteModDescriptorJson
// ============================================================================

bool UWhiskerwoodModToolsLibrary::WriteModDescriptorJson(
	const UPAL_WhiskerwoodModSettings* ModSettings,
	const FString& TargetFilePath)
{
	if (!ModSettings)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("WriteModDescriptorJson: ModSettings is null."));
		return false;
	}

	TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();

	FString Name = ModSettings->GetName();

	Name = ModSettings->ModName.IsEmpty()
		? ModSettings->GetName()
		: ModSettings->ModName;


	Root->SetStringField(TEXT("Name"), Name);


	Root->SetStringField(TEXT("Description"), ModSettings->Description);
	Root->SetStringField(TEXT("Version"), ModSettings->Version);
	Root->SetStringField(TEXT("CreatedBy"), ModSettings->CreatedBy);



	// Optional: include Steam App ID (global) and Workshop Id (per-mod)
	if (UWhiskerwoodEditorUserSettings* Settings = GetMutableDefault<UWhiskerwoodEditorUserSettings>())
	{
		if (Settings->SteamAppID > 0)
		{
			Root->SetNumberField(TEXT("SteamAppID"), Settings->SteamAppID);
		}
	}


	if (!ModSettings->SteamWorkshopId.IsEmpty())
	{
		Root->SetStringField(TEXT("SteamWorkshopId"), ModSettings->SteamWorkshopId);
	}


	FString Output;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);

	if (!FJsonSerializer::Serialize(Root, Writer))
	{
		UE_LOG(LogTemp, Error,
			TEXT("WriteModDescriptorJson: Failed to serialize JSON for mod '%s'."), *Name);
		return false;
	}

	if (!FFileHelper::SaveStringToFile(Output, *TargetFilePath))
	{
		UE_LOG(LogTemp, Error,
			TEXT("WriteModDescriptorJson: Failed to save JSON to '%s'."), *TargetFilePath);
		return false;
	}

	UE_LOG(LogTemp, Log,
		TEXT("WriteModDescriptorJson: Wrote descriptor for mod '%s' to '%s'."), *Name, *TargetFilePath);

	return true;
}

// ============================================================================
// MovePakMod
// ============================================================================

bool UWhiskerwoodModToolsLibrary::MovePakMod(UPAL_WhiskerwoodModSettings* ModSettings)
{
	if (!ModSettings)
	{
		UE_LOG(LogTemp, Warning, TEXT("MovePakMod: ModSettings is null."));
		return false;
	}


	// 1) Validate Editor User Settings
	UWhiskerwoodEditorUserSettings* Settings =
		GetMutableDefault<UWhiskerwoodEditorUserSettings>();

	if (!Settings)
	{
		UE_LOG(LogTemp, Error,
			TEXT("MovePakMod: Cannot access WhiskerwoodEditorUserSettings."));
		return false;
	}

	if (Settings->ProjectDirectory.IsEmpty())
	{
		UE_LOG(LogTemp, Error,
			TEXT("MovePakMod: ProjectDir is empty. Set this to the directory containing the .uproject."));
		return false;
	}

	if (Settings->PlatformName.IsEmpty())
	{
		UE_LOG(LogTemp, Warning,
			TEXT("MovePakMod: PlatformName is empty. Will not prefer any specific platform name in pak search."));
	}

	// 2) Validate ChunkId
	int32 ChunkId = -1;
#if WITH_EDITORONLY_DATA
	ChunkId = ModSettings->GetChunkId();
#endif

	if (ChunkId <= 0)
	{
		UE_LOG(LogTemp, Error,
			TEXT("MovePakMod: ChunkId is not set (<= 0) for mod '%s'."), *ModSettings->GetName());
		return false;
	}

	const FString ProjectDir   = Settings->ProjectDirectory;
	const FString PlatformName = Settings->PlatformName; // e.g. "Windows"

	// 3) Search for the pakchunk file anywhere under ProjectDir
	const FString PakPattern = FString::Printf(
		TEXT("pakchunk%d-*.pak"),
		ChunkId
	);

	UE_LOG(LogTemp, Log,
		TEXT("MovePakMod: Searching recursively under '%s' for '%s'"),
		*ProjectDir,
		*PakPattern);

	TArray<FString> FoundPaks;
	IFileManager& FileManager = IFileManager::Get();

	FileManager.FindFilesRecursive(
		FoundPaks,
		*ProjectDir,
		*PakPattern,
		/*Files*/ true,
		/*Directories*/ false
	);

	if (FoundPaks.Num() == 0)
	{
		UE_LOG(LogTemp, Error,
			TEXT("MovePakMod: Could not find any pak matching '%s' under '%s'."),
			*PakPattern,
			*ProjectDir);
		return false;
	}

	// Prefer a pak whose path contains the PlatformName (e.g., "Windows")
	FString SourcePakPath = FoundPaks[0];

	if (!PlatformName.IsEmpty())
	{
		for (const FString& Candidate : FoundPaks)
		{
			if (Candidate.Contains(PlatformName))
			{
				SourcePakPath = Candidate;
				break;
			}
		}
	}

	UE_LOG(LogTemp, Log,
		TEXT("MovePakMod: Using source pak: '%s'"), *SourcePakPath);

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (!PlatformFile.FileExists(*SourcePakPath))
	{
		UE_LOG(LogTemp, Error,
			TEXT("MovePakMod: Selected source pak does not exist: '%s'"), *SourcePakPath);
		return false;
	}

	// 4) Compute target mod directory under AppData/Local/Whiskerwood/Saved/mods
	FString ModDir = GetModDirectoryPath(ModSettings);

	if (!UWhiskerwoodFileIOLibrary::EnsureWhiskerwoodDirectory(ModDir))
	{
		UE_LOG(LogTemp, Error,
			TEXT("MovePakMod: Failed to ensure mod directory '%s'."), *ModDir);
		return false;
	}

	// 5) Target .pak name inside mod dir: <ModDirName>.pak
	FString ModDirName;
#if WITH_EDITORONLY_DATA
	ModDirName = ModSettings->ModDirName;
#endif
	if (ModDirName.IsEmpty())
	{
		ModDirName = ModSettings->GetName();
	}

	const FString TargetPakPath = FPaths::Combine(ModDir, ModDirName + TEXT(".pak"));

	UE_LOG(LogTemp, Log,
		TEXT("MovePakMod: Copying pak from '%s' to '%s'"),
		*SourcePakPath,
		*TargetPakPath);

	// 6) Copy / overwrite .pak into mod directory
	const bool bCopied = UWhiskerwoodFileIOLibrary::CopyWhiskerwoodFile(
		SourcePakPath,
		TargetPakPath,
		/*bOverwriteExisting*/ true);

	if (!bCopied)
	{
		UE_LOG(LogTemp, Error,
			TEXT("MovePakMod: Failed to copy pak for mod '%s'."), *ModDirName);
		return false;
	}

	// 7) Write .uplugin-style JSON descriptor into same directory
	const FString DescriptorPath = FPaths::Combine(ModDir, ModDirName + TEXT(".uplugin"));
	if (!WriteModDescriptorJson(ModSettings, DescriptorPath))
	{
		UE_LOG(LogTemp, Error,
			TEXT("MovePakMod: Failed to write descriptor for mod '%s'."), *ModDirName);
		return false;
	}

	UE_LOG(LogTemp, Log,
		TEXT("MovePakMod: Successfully deployed mod '%s' to '%s'."),
		*ModDirName, *ModDir);

	return true;

}

// ============================================================================
// RunPackagingForMod
// ============================================================================

bool UWhiskerwoodModToolsLibrary::RunPackagingForMod()
{

	// This runs Unreal Automation Tool (UAT) as an external process:
	// RunUAT.bat BuildCookRun -project="..." -pak -package -platform=Win64 -cook ...
	// Returns true ONLY if UAT exits with code 0.

	// 1) Find RunUAT.bat
	const FString EngineDir = FPaths::ConvertRelativePathToFull(FPaths::EngineDir());
	const FString UATPath   = FPaths::Combine(EngineDir, TEXT("Build"), TEXT("BatchFiles"), TEXT("RunUAT.bat"));

	if (!FPaths::FileExists(UATPath))
	{
		UE_LOG(LogTemp, Error,
			TEXT("RunPackagingForMod: RunUAT.bat not found at '%s'"), *UATPath);
		return false;
	}

	// 2) Get the .uproject path
	const FString ProjectFile = FPaths::ConvertRelativePathToFull(FPaths::GetProjectFilePath());
	if (ProjectFile.IsEmpty())
	{
		UE_LOG(LogTemp, Error,
			TEXT("RunPackagingForMod: Project file path is empty."));
		return false;
	}

	// 3) Pull platform info from settings
	UWhiskerwoodEditorUserSettings* Settings =
		GetMutableDefault<UWhiskerwoodEditorUserSettings>();

	FString UserPlatformName = TEXT("Windows");
	if (Settings && !Settings->PlatformName.IsEmpty())
	{
		UserPlatformName = Settings->PlatformName;
	}

	// What UAT actually wants
	FString UATPlatformName = TEXT("Win64");

	if (UserPlatformName.Equals(TEXT("Win64"), ESearchCase::IgnoreCase))
	{
		UATPlatformName = TEXT("Win64");
	}
	else if (UserPlatformName.Equals(TEXT("Windows"), ESearchCase::IgnoreCase) ||
	         UserPlatformName.Equals(TEXT("WindowsNoEditor"), ESearchCase::IgnoreCase))
	{
		UATPlatformName = TEXT("Win64");
	}

	// 4) Build UAT command line
	const FString CommandLine = FString::Printf(
		TEXT("BuildCookRun -project=\"%s\" -noP4 -clientconfig=Development -serverconfig=Development ")
		TEXT("-nocompile -stage -pak -package -platform=%s -cook -skipeditorcontent"),
		*ProjectFile,
		*UATPlatformName
	);

	UE_LOG(LogTemp, Log,
		TEXT("RunPackagingForMod: Starting UAT:\n  \"%s\" %s"),
		*UATPath,
		*CommandLine);

	uint32 ProcID = 0;

	FProcHandle ProcHandle = FPlatformProcess::CreateProc(
		*UATPath,
		*CommandLine,
		/*bLaunchDetached*/ false,
		/*bLaunchHidden*/  false,
		/*bLaunchReallyHidden*/ false,
		&ProcID,
		0,
		nullptr,
		nullptr
	);

	if (!ProcHandle.IsValid())
	{
		UE_LOG(LogTemp, Error,
			TEXT("RunPackagingForMod: Failed to start UAT process."));
		return false;
	}

	// Block until UAT finishes
	while (FPlatformProcess::IsProcRunning(ProcHandle))
	{
		FPlatformProcess::Sleep(1.0f);
	}

	int32 ReturnCode = -1;
	FPlatformProcess::GetProcReturnCode(ProcHandle, &ReturnCode);

	const bool bSuccess = (ReturnCode == 0);

	UE_LOG(LogTemp, Log,
		TEXT("RunPackagingForMod: UAT finished with exit code %d (Success=%s)"),
		ReturnCode,
		bSuccess ? TEXT("true") : TEXT("false"));

	if (!bSuccess)
	{
		UE_LOG(LogTemp, Error,
			TEXT("RunPackagingForMod: UAT reported failure. Check the AutomationTool log under:")
			TEXT("  %%AppData%%/Unreal Engine/AutomationTool/Logs/"));
	}

	return bSuccess;

}

// ============================================================================
// Status helpers
// ============================================================================

bool UWhiskerwoodModToolsLibrary::HasSourcePak(const UPAL_WhiskerwoodModSettings* ModSettings)
{
	if (!ModSettings)
	{
		return false;
	}

	int32 ChunkId = -1;
#if WITH_EDITORONLY_DATA
	ChunkId = ModSettings->GetChunkId();
#endif

	if (ChunkId <= 0)
	{
		return false;
	}

	UWhiskerwoodEditorUserSettings* Settings =
		GetMutableDefault<UWhiskerwoodEditorUserSettings>();

	if (!Settings || Settings->ProjectDirectory.IsEmpty())
	{
		return false;
	}

	const FString ProjectDir = Settings->ProjectDirectory;

	const FString PakPattern = FString::Printf(
		TEXT("pakchunk%d-*.pak"),
		ChunkId
	);

	TArray<FString> FoundPaks;
	IFileManager& FileManager = IFileManager::Get();

	FileManager.FindFilesRecursive(
		FoundPaks,
		*ProjectDir,
		*PakPattern,
		/*Files*/ true,
		/*Directories*/ false
	);

	return FoundPaks.Num() > 0;
}

bool UWhiskerwoodModToolsLibrary::IsModMoved(const UPAL_WhiskerwoodModSettings* ModSettings)
{
	if (!ModSettings)
	{
		return false;
	}

	FString ModDir = GetModDirectoryPath(ModSettings);
	if (ModDir.IsEmpty())
	{
		return false;
	}

	FString ModDirName;
#if WITH_EDITORONLY_DATA
	ModDirName = ModSettings->ModDirName;
#endif
	if (ModDirName.IsEmpty())
	{
		ModDirName = ModSettings->GetName();
	}

	const FString PakPath = FPaths::Combine(ModDir, ModDirName + TEXT(".pak"));

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	return PlatformFile.FileExists(*PakPath);
}


bool UWhiskerwoodModToolsLibrary::CanDeployToWorkshop(const UPAL_WhiskerwoodModSettings* ModSettings)
{
	if (!IsModMoved(ModSettings))
	{
		return false;
	}

	UWhiskerwoodEditorUserSettings* Settings =
		GetMutableDefault<UWhiskerwoodEditorUserSettings>();

	if (!Settings)
	{
		return false;
	}

	return Settings->SteamAppID > 0;
}

// ============================================================================
// Remove / Deploy
// ============================================================================

bool UWhiskerwoodModToolsLibrary::RemoveMovedMod(UPAL_WhiskerwoodModSettings* ModSettings)
{
	if (!ModSettings)
	{
		return false;
	}

	if (!IsModMoved(ModSettings))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("RemoveMovedMod: Mod '%s' is not moved; nothing to remove."),
			*ModSettings->GetName());
		return false;
	}

	FString ModDir = GetModDirectoryPath(ModSettings);
	if (ModDir.IsEmpty())
	{
		return false;
	}

	return UWhiskerwoodFileIOLibrary::DeleteWhiskerwoodDirectory(ModDir);
}

bool UWhiskerwoodModToolsLibrary::DeployModToWorkshopStaging(
	UPAL_WhiskerwoodModSettings* ModSettings,
	FString& OutStagingDir)
{
	OutStagingDir.Empty();

	if (!ModSettings)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("DeployModToWorkshopStaging: ModSettings is null."));
		return false;
	}

	if (!IsModMoved(ModSettings))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("DeployModToWorkshopStaging: Mod '%s' is not moved; Move to Saved/mods first."),
			*ModSettings->GetName());
		return false;
	}

	// Compute staging root based off the same LocalAppData/Whiskerwood root
	const FString ModsDir = UWhiskerwoodFileIOLibrary::GetWhiskerwoodModsDirectory();
	if (ModsDir.IsEmpty())
	{
		return false;
	}

	// ModsDir = <LocalAppData>/Whiskerwood/Saved/mods
	// Step up twice to get <LocalAppData>/Whiskerwood
	const FString SavedDir = FPaths::GetPath(ModsDir);      // .../Whiskerwood/Saved
	const FString GameRootDir = FPaths::GetPath(SavedDir);  // .../Whiskerwood

	const FString WorkshopRoot = FPaths::Combine(GameRootDir, TEXT("WorkshopStaging"));

	FString ModDirName;
#if WITH_EDITORONLY_DATA
	ModDirName = ModSettings->ModDirName;
#endif
	if (ModDirName.IsEmpty())
	{
		ModDirName = ModSettings->GetName();
	}

	const FString StagingDir = FPaths::Combine(WorkshopRoot, ModDirName);

	if (!UWhiskerwoodFileIOLibrary::EnsureWhiskerwoodDirectory(StagingDir))
	{
		UE_LOG(LogTemp, Error,
			TEXT("DeployModToWorkshopStaging: Failed to ensure staging directory '%s'."), *StagingDir);
		return false;
	}

	const FString SourceModDir = GetModDirectoryPath(ModSettings);
	const FString SourcePakPath = FPaths::Combine(SourceModDir, ModDirName + TEXT(".pak"));
	const FString SourceDescriptorPath = FPaths::Combine(SourceModDir, ModDirName + TEXT(".uplugin"));

	const FString TargetPakPath = FPaths::Combine(StagingDir, ModDirName + TEXT(".pak"));
	const FString TargetDescriptorPath = FPaths::Combine(StagingDir, ModDirName + TEXT(".uplugin"));

	bool bOk = true;

	// Copy pak
	if (!UWhiskerwoodFileIOLibrary::CopyWhiskerwoodFile(
			SourcePakPath,
			TargetPakPath,
			/*bOverwriteExisting*/ true))
	{
		UE_LOG(LogTemp, Error,
			TEXT("DeployModToWorkshopStaging: Failed to copy pak from '%s' to '%s'."),
			*SourcePakPath, *TargetPakPath);
		bOk = false;
	}

	// Copy descriptor (if it exists)
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (PlatformFile.FileExists(*SourceDescriptorPath))
	{
		if (!UWhiskerwoodFileIOLibrary::CopyWhiskerwoodFile(
				SourceDescriptorPath,
				TargetDescriptorPath,
				/*bOverwriteExisting*/ true))
		{
			UE_LOG(LogTemp, Error,
				TEXT("DeployModToWorkshopStaging: Failed to copy descriptor from '%s' to '%s'."),
				*SourceDescriptorPath, *TargetDescriptorPath);
			bOk = false;
		}
	}

	if (!bOk)
	{
		return false;
	}

	OutStagingDir = StagingDir;

	UE_LOG(LogTemp, Log,
		TEXT("DeployModToWorkshopStaging: Staged mod '%s' to '%s'."),
		*ModDirName, *StagingDir);

	return true;
}
