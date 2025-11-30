#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WhiskerwoodModToolsLibrary.generated.h"

class UPDA_WhiskerwoodModSettings;
class UWhiskerwoodModListItem;


/**
 * Editor-only modding tools for Whiskerwood.
 */
UCLASS()
class WHISKERWOOD_API UWhiskerwoodModToolsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
		* Finds all UPDA_WhiskerwoodModSettings assets in the project and
		* builds UWhiskerwoodModListItem objects for a ListView.
		*/
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|Modding", meta=(DevelopmentOnly))
	static void GetAllModListItems(TArray<UWhiskerwoodModListItem*>& OutItems);

	/**
	 * Finds and loads all UPDA_WhiskerwoodModSettings assets in the project.
	 * Editor-only (uses AssetRegistry).
	 */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|Modding", meta=(DevelopmentOnly))
	static void GetAllWhiskerwoodModSettings(TArray<UPDA_WhiskerwoodModSettings*>& OutModSettings);

	/**
	 * Builds the full disk path to the mod directory for a given mod settings asset.
	 *
	 * Example result:
	 *   C:/Users/<User>/AppData/Local/Whiskerwood/Saved/mods/MoreStarterWhiskers
	 */
	UFUNCTION(BlueprintPure, Category="Whiskerwood|Modding", meta=(DevelopmentOnly))
	static FString GetModDirectoryPath(const UPDA_WhiskerwoodModSettings* ModSettings);

	/**
	 * Writes a minimal .uplugin-style JSON descriptor for the given mod
	 * into the target file path.
	 *
	 * The JSON will look like:
	 * {
	 *     "Name" : "More Starter Whiskers",
	 *     "Description" : "...",
	 *     "Version" : "1.0",
	 *     "CreatedBy" : "Shenjima"
	 * }
	 */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|Modding", meta=(DevelopmentOnly))
	static bool WriteModDescriptorJson(const UPDA_WhiskerwoodModSettings* ModSettings,
	                                   const FString& TargetFilePath);

	/**
 * Validates settings and ChunkID, then:
 *  - Builds the expected .pak path from EditorUserSettings->PlatformDir and ChunkID
 *  - Ensures the mod directory exists under AppData/Local/Whiskerwood/Saved/mods
 *  - Copies/overwrites the .pak to <ModDir>/<ModDirName>.pak
 *  - Writes <ModDir>/<ModDirName>.uplugin JSON descriptor
 *
 * NOTE: This does NOT (yet) run the full UE "Platforms > Windows > Package Project".
 * It assumes the .pak for the given ChunkID already exists in PlatformDir.
 */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|Modding", meta=(DevelopmentOnly))
	static bool PakAndMoveMod(UPDA_WhiskerwoodModSettings* ModSettings);

	UFUNCTION(BlueprintCallable, Category="Whiskerwood|Modding", meta=(DevelopmentOnly))
	static bool RunPackagingForMod();

};
