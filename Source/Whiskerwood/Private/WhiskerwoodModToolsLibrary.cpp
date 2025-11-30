#include "WhiskerwoodModToolsLibrary.h"

#include "WhiskerwoodModListItem.h"
#include "PDA_WhiskerwoodModSettings.h"
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
    TArray<UPDA_WhiskerwoodModSettings*>& OutModSettings)
{
#if WITH_EDITOR
    OutModSettings.Reset();

    FAssetRegistryModule& AssetRegistryModule =
        FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    FARFilter Filter;
    Filter.bRecursiveClasses = true;
    Filter.bIncludeOnlyOnDiskAssets = false;

    // UE5.6+: use ClassPaths instead of ClassNames
    Filter.ClassPaths.Add(UPDA_WhiskerwoodModSettings::StaticClass()->GetClassPathName());

    TArray<FAssetData> AssetDataList;
    AssetRegistry.GetAssets(Filter, AssetDataList);

    for (const FAssetData& AssetData : AssetDataList)
    {
        if (UObject* LoadedObj = AssetData.GetAsset())
        {
            if (UPDA_WhiskerwoodModSettings* ModSettings =
                    Cast<UPDA_WhiskerwoodModSettings>(LoadedObj))
            {
                OutModSettings.Add(ModSettings);
            }
        }
    }
#else
    // Non-editor build: return empty, but symbol must exist for linker.
    OutModSettings.Reset();
#endif
}

// ============================================================================
// GetAllModListItems
// ============================================================================

void UWhiskerwoodModToolsLibrary::GetAllModListItems(
    TArray<UWhiskerwoodModListItem*>& OutItems)
{
#if WITH_EDITOR
    OutItems.Reset();

    FAssetRegistryModule& AssetRegistryModule =
        FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    FARFilter Filter;
    Filter.bRecursiveClasses = true;
    Filter.bIncludeOnlyOnDiskAssets = false;
    Filter.ClassPaths.Add(UPDA_WhiskerwoodModSettings::StaticClass()->GetClassPathName());

    TArray<FAssetData> AssetDataList;
    AssetRegistry.GetAssets(Filter, AssetDataList);

    for (const FAssetData& AssetData : AssetDataList)
    {
        UObject* LoadedObj = AssetData.GetAsset();
        if (!LoadedObj)
        {
            continue;
        }

        UPDA_WhiskerwoodModSettings* ModSettings =
            Cast<UPDA_WhiskerwoodModSettings>(LoadedObj);

        if (!ModSettings)
        {
            continue;
        }

        UWhiskerwoodModListItem* Item = NewObject<UWhiskerwoodModListItem>();
        Item->ModSettings = ModSettings;
        Item->ModName = ModSettings->ModName.IsEmpty()
            ? ModSettings->GetName()
            : ModSettings->ModName;
        Item->ChunkID = ModSettings->ChunkID;
        Item->ModDirectoryPath = GetModDirectoryPath(ModSettings);

        OutItems.Add(Item);
    }
#else
    OutItems.Reset();
#endif
}

// ============================================================================
// GetModDirectoryPath
// ============================================================================

FString UWhiskerwoodModToolsLibrary::GetModDirectoryPath(
    const UPDA_WhiskerwoodModSettings* ModSettings)
{
#if WITH_EDITOR
    if (!ModSettings)
    {
        return TEXT("");
    }

    const FString BaseModsDir = UWhiskerwoodFileIOLibrary::GetWhiskerwoodModsDirectory();

    FString DirName = ModSettings->ModDirName;
    if (DirName.IsEmpty())
    {
        // Fallback: use asset name
        DirName = ModSettings->GetName();
    }

    FString FullPath = FPaths::Combine(BaseModsDir, DirName);
    FPaths::NormalizeDirectoryName(FullPath);

    return FullPath;
#else
    return FString();
#endif
}

// ============================================================================
// WriteModDescriptorJson
// ============================================================================

bool UWhiskerwoodModToolsLibrary::WriteModDescriptorJson(
    const UPDA_WhiskerwoodModSettings* ModSettings,
    const FString& TargetFilePath)
{
#if WITH_EDITOR
    if (!ModSettings)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("WriteModDescriptorJson: ModSettings is null."));
        return false;
    }

    TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();

    const FString Name = ModSettings->ModName.IsEmpty()
        ? ModSettings->GetName()
        : ModSettings->ModName;

    Root->SetStringField(TEXT("Name"), Name);
    Root->SetStringField(TEXT("Description"), ModSettings->Description);
    Root->SetStringField(TEXT("Version"), ModSettings->Version);
    Root->SetStringField(TEXT("CreatedBy"), ModSettings->CreatedBy);

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
#else
    UE_LOG(LogTemp, Warning,
        TEXT("WriteModDescriptorJson is editor-only in non-editor builds."));
    return false;
#endif
}

// ============================================================================
// PakAndMoveMod
// ============================================================================

bool UWhiskerwoodModToolsLibrary::PakAndMoveMod(UPDA_WhiskerwoodModSettings* ModSettings)
{
#if WITH_EDITOR
    if (!ModSettings)
    {
        UE_LOG(LogTemp, Warning, TEXT("PakAndMoveMod: ModSettings is null."));
        return false;
    }

    // 1) Validate Editor User Settings
    UWhiskerwoodEditorUserSettings* Settings =
        GetMutableDefault<UWhiskerwoodEditorUserSettings>();

    if (!Settings)
    {
        UE_LOG(LogTemp, Error,
            TEXT("PakAndMoveMod: Cannot access WhiskerwoodEditorUserSettings."));
        return false;
    }

    if (Settings->ProjectDir.IsEmpty())
    {
        UE_LOG(LogTemp, Error,
            TEXT("PakAndMoveMod: ProjectDir is empty. Set this to the directory containing the .uproject."));
        return false;
    }

    if (Settings->PlatformDir.IsEmpty())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("PakAndMoveMod: PlatformDir is empty. Will not prefer any specific platform name in pak search."));
    }

    if (Settings->AppDataDir.IsEmpty())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("PakAndMoveMod: AppDataDir is empty; using GetWhiskerwoodModsDirectory instead."));
    }

    // 2) Validate ChunkID
    if (ModSettings->ChunkID <= 0)
    {
        UE_LOG(LogTemp, Error,
            TEXT("PakAndMoveMod: ChunkID is not set (<= 0) for mod '%s'."), *ModSettings->GetName());
        return false;
    }

    const FString ProjectDir   = Settings->ProjectDir;
    const FString PlatformName = Settings->PlatformDir; // e.g. "Windows"

    // 3) Search for the pakchunk file anywhere under ProjectDir
    const FString PakPattern = FString::Printf(
        TEXT("pakchunk%d-*.pak"),
        ModSettings->ChunkID
    );

    UE_LOG(LogTemp, Log,
        TEXT("PakAndMoveMod: Searching recursively under '%s' for '%s'"),
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
            TEXT("PakAndMoveMod: Could not find any pak matching '%s' under '%s'.\n")
            TEXT("Make sure packaging generated a chunk pak for ChunkID %d."),
            *PakPattern,
            *ProjectDir,
            ModSettings->ChunkID);
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
        TEXT("PakAndMoveMod: Using source pak: '%s'"), *SourcePakPath);

    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    if (!PlatformFile.FileExists(*SourcePakPath))
    {
        UE_LOG(LogTemp, Error,
            TEXT("PakAndMoveMod: Selected source pak does not exist: '%s'"), *SourcePakPath);
        return false;
    }

    // 4) Compute target mod directory under AppData/Local/Whiskerwood/Saved/mods
    FString ModDir = GetModDirectoryPath(ModSettings);

    if (!UWhiskerwoodFileIOLibrary::EnsureWhiskerwoodDirectory(ModDir))
    {
        UE_LOG(LogTemp, Error,
            TEXT("PakAndMoveMod: Failed to ensure mod directory '%s'."), *ModDir);
        return false;
    }

    // 5) Target .pak name inside mod dir: <ModDirName>.pak
    FString ModDirName = ModSettings->ModDirName;
    if (ModDirName.IsEmpty())
    {
        ModDirName = ModSettings->GetName();
    }

    const FString TargetPakPath = FPaths::Combine(ModDir, ModDirName + TEXT(".pak"));

    UE_LOG(LogTemp, Log,
        TEXT("PakAndMoveMod: Copying pak from '%s' to '%s'"),
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
            TEXT("PakAndMoveMod: Failed to copy pak for mod '%s'."), *ModDirName);
        return false;
    }

    // 7) Write .uplugin-style JSON descriptor into same directory
    const FString DescriptorPath = FPaths::Combine(ModDir, ModDirName + TEXT(".uplugin"));
    if (!WriteModDescriptorJson(ModSettings, DescriptorPath))
    {
        UE_LOG(LogTemp, Error,
            TEXT("PakAndMoveMod: Failed to write descriptor for mod '%s'."), *ModDirName);
        return false;
    }

    UE_LOG(LogTemp, Log,
        TEXT("PakAndMoveMod: Successfully deployed mod '%s' to '%s'."),
        *ModDirName, *ModDir);

    return true;
#else
    UE_LOG(LogTemp, Warning,
        TEXT("PakAndMoveMod is editor-only in non-editor builds."));
    return false;
#endif
}

// ============================================================================
// RunPackagingForMod
// ============================================================================

bool UWhiskerwoodModToolsLibrary::RunPackagingForMod()
{
#if WITH_EDITOR
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

    // 3) Pull platform info from settings, but translate to a valid UAT platform
    UWhiskerwoodEditorUserSettings* Settings =
        GetMutableDefault<UWhiskerwoodEditorUserSettings>();

    // What the user types / what we use in path preference
    FString UserPlatformName = TEXT("Windows");
    if (Settings && !Settings->PlatformDir.IsEmpty())
    {
        UserPlatformName = Settings->PlatformDir;
    }

    // What UAT actually wants
    FString UATPlatformName = TEXT("Win64");

    // If you later support more, you can expand this mapping.
    if (!UserPlatformName.IsEmpty())
    {
        if (UserPlatformName.Equals(TEXT("Win64"), ESearchCase::IgnoreCase))
        {
            UATPlatformName = TEXT("Win64");
        }
        else if (UserPlatformName.Equals(TEXT("Windows"), ESearchCase::IgnoreCase) ||
                 UserPlatformName.Equals(TEXT("WindowsNoEditor"), ESearchCase::IgnoreCase))
        {
            UATPlatformName = TEXT("Win64");
        }
        // else: you could add more mappings or just trust the user
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
#else
    UE_LOG(LogTemp, Warning,
        TEXT("RunPackagingForMod is editor-only in non-editor builds."));
    return false;
#endif
}

