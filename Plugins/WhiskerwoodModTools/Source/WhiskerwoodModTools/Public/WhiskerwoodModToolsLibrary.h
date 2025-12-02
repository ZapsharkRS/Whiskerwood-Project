#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WhiskerwoodModToolsLibrary.generated.h"

class UPAL_WhiskerwoodModSettings;
class UWhiskerwoodModListItem;

/**
 * Editor-only modding tools for Whiskerwood.
 *
 * All of these are meant for Editor Utility Widgets / Editor scripting.
 */
UCLASS()
class WHISKERWOODMODTOOLS_API UWhiskerwoodModToolsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// --------------------------------------------------------------------
	// Resolve helpers (smart defaults + overrides)
	// --------------------------------------------------------------------

	/** Resolve Project Directory (settings override → current project dir). */
	UFUNCTION(BlueprintPure, Category="Whiskerwood|ModUtils|Resolve")
	static FString ResolveProjectDirectory();

	/** Resolve Mods Directory (settings override → LOCALAPPDATA/Whiskerwood/Saved/mods). */
	UFUNCTION(BlueprintPure, Category="Whiskerwood|ModUtils|Resolve")
	static FString ResolveModsDirectory();

	/** Resolve App Data Directory (settings override → derived from Mods dir). */
	UFUNCTION(BlueprintPure, Category="Whiskerwood|ModUtils|Resolve")
	static FString ResolveAppDataDirectory();

	/** Resolve Base Logs Directory (settings override → AppData/Saved/Logs). */
	UFUNCTION(BlueprintPure, Category="Whiskerwood|ModUtils|Resolve")
	static FString ResolveBaseLogsDirectory();

	/** Resolve Temp Deploy Directory (settings override → AppData/WorkshopTemp). */
	UFUNCTION(BlueprintPure, Category="Whiskerwood|ModUtils|Resolve")
	static FString ResolveTempDeployDirectory();

	/**
	 * Resolve Pak Directory:
	 *  - Settings.PakDirectory if set
	 *  - Otherwise, try to auto-detect by scanning for pakchunk*.pak under ProjectDirectory
	 *    and returning the first directory that contains them.
	 *  - Returns empty string if nothing is found.
	 */
	UFUNCTION(BlueprintPure, Category="Whiskerwood|ModUtils|Resolve")
	static FString ResolvePakDirectory();

	/**
	 * Map human PlatformName ("Windows", "Linux", etc.) to UAT platform argument
	 * ("Win64", "Linux", etc.).
	 */
	UFUNCTION(BlueprintPure, Category="Whiskerwood|ModUtils|Resolve")
	static FString ResolveUATPlatformName();

	// --------------------------------------------------------------------
	// Validation helpers (for UI indicators)
	// --------------------------------------------------------------------

	/** True if the string looks like a non-empty path (cheap sanity check). */
	UFUNCTION(BlueprintPure, Category="Whiskerwood|ModUtils|Validate")
	static bool IsNonEmptyPath(const FString& Path);

	/** True if the directory exists on disk. */
	UFUNCTION(BlueprintPure, Category="Whiskerwood|ModUtils|FileIO")
	static bool DoesDirectoryExist(const FString& DirPath);

	/** True if the file exists on disk. */
	UFUNCTION(BlueprintPure, Category="Whiskerwood|ModUtils|FileIO")
	static bool DoesFileExist(const FString& FilePath);
	
	/**
	 * Finds all UPAL_WhiskerwoodModSettings assets in the project and
	 * builds UWhiskerwoodModListItem objects for a ListView.
	 */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|Widget Row", meta=(DevelopmentOnly))
	static void GetAllModListItems(TArray<UWhiskerwoodModListItem*>& OutItems);

	/**
	 * Finds and loads all UPAL_WhiskerwoodModSettings assets in the project.
	 * Editor-only (uses AssetRegistry).
	 */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|Settings", meta=(DevelopmentOnly))
	static void GetAllWhiskerwoodModSettings(TArray<UPAL_WhiskerwoodModSettings*>& OutModSettings);

	/**
	 * Builds the full disk path to the mod directory for a given mod PAL.
	 *
	 * Example result:
	 *   C:/Users/<User>/AppData/Local/Whiskerwood/Saved/mods/MoreStarterWhiskers
	 */
	UFUNCTION(BlueprintPure, Category="Whiskerwood|ModUtils|Settings", meta=(DevelopmentOnly))
	static FString GetModDirectoryPath(const UPAL_WhiskerwoodModSettings* ModSettings);

	/**
	 * Writes a minimal .uplugin-style JSON descriptor for the given mod
	 * into the target file path.
	 *
	 * The JSON will look like:
	 * {
	 *     "Name" : "More Starter Whiskers",
	 *     "Description" : "...",
	 *     "Version" : "1.0",
	 *     "CreatedBy" : "Shenjima",
	 *     "SteamAppID" : 123456,
	 *     "SteamWorkshopId" : "9876543210"
	 * }
	 */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|Settings", meta=(DevelopmentOnly))
	static bool WriteModDescriptorJson(const UPAL_WhiskerwoodModSettings* ModSettings,
	                                   const FString& TargetFilePath);

	/**
	 * Validates settings and ChunkID, then:
	 *  - Searches under ProjectDir for pakchunk<ChunkID>-*.pak
	 *  - Ensures the mod directory exists under AppData/Local/Whiskerwood/Saved/mods
	 *  - Copies/overwrites the .pak to <ModDir>/<ModDirName>.pak
	 *  - Writes <ModDir>/<ModDirName>.uplugin JSON descriptor
	 *
	 * NOTE: This does NOT run packaging. It assumes the .pak for the given
	 * ChunkID already exists (e.g., after a global "Pak" run).
	 */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|Pak", meta=(DevelopmentOnly))
	static bool MovePakMod(UPAL_WhiskerwoodModSettings* ModSettings);

	/**
	 * Runs UAT once to build/cook/package the project and generate all
	 * chunk paks (pakchunkX-*.pak).
	 *
	 * Intended for a single top-level "Pak" button in the dashboard.
	 */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|Pak", meta=(DevelopmentOnly))
	static bool RunPackagingForMod();

	// ---------------------------------------------------------------------
	// Row status helpers for the Dashboard List Grid
	// ---------------------------------------------------------------------

	/** True if a pakchunk<ChunkID>-*.pak exists somewhere under ProjectDir. */
	UFUNCTION(BlueprintPure, Category="Whiskerwood|ModUtils|Widget Row|Status", meta=(DevelopmentOnly))
	static bool HasSourcePak(const UPAL_WhiskerwoodModSettings* ModSettings);

	/** True if <ModsDir>/<ModDirName>/<ModDirName>.pak exists (i.e., deployed). */
	UFUNCTION(BlueprintPure, Category="Whiskerwood|ModUtils|Widget Row|Status", meta=(DevelopmentOnly))
	static bool IsModMoved(const UPAL_WhiskerwoodModSettings* ModSettings);

	/**
	 * True if the mod is deployed AND a SteamAppID is configured.
	 * Use this to gate-enable a "Deploy" button.
	 */
	UFUNCTION(BlueprintPure, Category="Whiskerwood|ModUtils|Widget Row|Status", meta=(DevelopmentOnly))
	static bool CanDeployToWorkshop(const UPAL_WhiskerwoodModSettings* ModSettings);

	// ---------------------------------------------------------------------
	// Actions for Move / Remove / Deploy buttons
	// ---------------------------------------------------------------------

	/**
	 * Removes the deployed mod directory:
	 *   <AppData>/Local/Whiskerwood/Saved/mods/<ModDirName>/
	 */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|Widget Row", meta=(DevelopmentOnly))
	static bool RemoveMovedMod(UPAL_WhiskerwoodModSettings* ModSettings);

	/**
	 * Copies the deployed mod files (pak + .uplugin) into a Workshop staging
	 * directory under the same Whiskerwood LocalAppData root.
	 *
	 * Resulting path:
	 *   <LocalAppData>/Whiskerwood/WorkshopStaging/<ModDirName>/
	 *
	 * Returns true on success and fills OutStagingDir with the full path.
	 */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|Widget Row|Steam", meta=(DevelopmentOnly))
	static bool DeployModToWorkshopStaging(UPAL_WhiskerwoodModSettings* ModSettings,
	                                       FString& OutStagingDir);
};
