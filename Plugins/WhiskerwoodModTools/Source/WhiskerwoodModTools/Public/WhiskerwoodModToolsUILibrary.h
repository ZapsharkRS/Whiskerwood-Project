#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WhiskerwoodModToolsUILibrary.generated.h"

class UTextBlock;
class UEditableTextBox;
class UButton;
class UWhiskerwoodModListItem;
class UPAL_WhiskerwoodModSettings;

UENUM(BlueprintType)
enum class EWhiskerwoodSpecialDirectory : uint8
{
	Mods        UMETA(DisplayName="Mods Directory"),
	Logs        UMETA(DisplayName="Logs Directory"),
	TempWorkshop UMETA(DisplayName="Temp Workshop Directory")
};

/**
 * UI helper library for Whiskerwood Mod Tools.
 *
 * These are "one node" helpers to keep your Blueprints clean:
 *  - Settings panel: update text + colors from settings + autodetect.
 *  - Mod row: update labels + status + button enabled state from ModItem.
 */
UCLASS()
class WHISKERWOODMODTOOLS_API UWhiskerwoodModToolsUILibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|UI")
	static void RefreshSettingsBasicUI(
		UTextBlock* ProjectLabel,
		UEditableTextBox* ProjectTextBox,
		UTextBlock* AppDataLabel,
		UEditableTextBox* AppDataTextBox,
		UTextBlock* ModsLabel,
		UEditableTextBox* ModsTextBox,
		UTextBlock* PlatformLabel,
		UEditableTextBox* PlatformTextBox,
		UTextBlock* SteamAppIDLabel,
		UEditableTextBox* SteamAppIDTextBox,
		UTextBlock* TempDeployLabel,
		UEditableTextBox* TempDeployTextBox
	);

	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|UI")
	static void SaveSettingsFromBasicUI(
		UEditableTextBox* ProjectTextBox,
		UEditableTextBox* AppDataTextBox,
		UEditableTextBox* ModsTextBox,
		UEditableTextBox* PlatformTextBox,
		UEditableTextBox* SteamAppIDTextBox,
		UEditableTextBox* TempDeployTextBox
	);

	// --------------------------------------------------------------------
	// SETTINGS PANEL HELPER
	// --------------------------------------------------------------------

	/**
	 * Refreshes the Settings UI for core paths.
	 *
	 * - If a TextBox is empty, it is filled with the resolved default
	 *   (ResolveXDirectory()).
	 * - Each Label is colored green if the directory exists, red otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|UI")
	static void RefreshSettingsPathsUI(
		// Project
		UTextBlock* ProjectLabel,
		UEditableTextBox* ProjectTextBox,

		// Mods
		UTextBlock* ModsLabel,
		UEditableTextBox* ModsTextBox,

		// AppData root
		UTextBlock* AppDataLabel,
		UEditableTextBox* AppDataTextBox,

		// Base Logs
		UTextBlock* LogsLabel,
		UEditableTextBox* LogsTextBox,

		// Temp Deploy
		UTextBlock* TempDeployLabel,
		UEditableTextBox* TempDeployTextBox,

		// Pak directory (optional – can be empty if no paks yet)
		UTextBlock* PakLabel,
		UEditableTextBox* PakTextBox
	);

	// --------------------------------------------------------------------
	// MOD LIST ROW HELPER
	// --------------------------------------------------------------------

	/**
	 * Refreshes a Mod List Row UI based on its ModItem.
	 *
	 * - Sets Name and ChunkId text.
	 * - Computes a Status string + color:
	 *     "Missing ChunkID", "No Pak Found", "Ready to be Moved",
	 *     "Moved | Ready for Deploy", "Moved | Steam Not Configured", etc.
	 * - Enables/disables Move, Remove, Deploy buttons:
	 *     Move   -> HasSourcePak && !IsModMoved
	 *     Remove -> IsModMoved
	 *     Deploy -> IsModMoved && CanDeployToWorkshop
	 */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|UI")
	static void RefreshModListRowUI(
		UWhiskerwoodModListItem* ModItem,
		UTextBlock* NameText,
		UTextBlock* ChunkIdText,
		UTextBlock* StatusText,
		UButton* MoveButton,
		UButton* RemoveButton,
		UButton* DeployButton
	);

	/**
	 * Sets the Temp Deploy directory to a default "TempWorkshop" folder
	 * under the resolved AppData directory.
	 *
	 * Example:
	 *   AppDataDirectory = C:/Users/<User>/AppData/Local/Whiskerwood
	 *   TempWorkshopDir  = C:/Users/<User>/AppData/Local/Whiskerwood/TempWorkshop
	 *
	 * This helper:
	 *  - Computes that path using ResolveAppDataDirectory().
	 *  - Creates the directory if it doesn't exist.
	 *  - Updates the TempDeployTextBox.
	 *  - Writes the value into WhiskerwoodEditorUserSettings::TempDeployDirectory.
	 *  - Colors the TempDeployLabel green if creation/exists succeeded, red otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|UI")
	static void SetTempDeployToWorkshopDefault(
		UTextBlock* TempDeployLabel,
		UEditableTextBox* TempDeployTextBox
	);

	// Generic open/browse helpers

	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|UI", meta=(DevelopmentOnly))
	static void OpenDirectoryFromTextBox(
		UEditableTextBox* SourceTextBox,
		bool bCreateIfMissing
	);

	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|UI", meta=(DevelopmentOnly))
	static void BrowseForDirectoryIntoTextBox(
		UEditableTextBox* TargetTextBox,
		FText DialogTitle
	);

	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|UI", meta=(DevelopmentOnly))
	static void OpenWhiskerwoodSpecialDirectory(
		EWhiskerwoodSpecialDirectory DirectoryType,
		bool bCreateIfMissing
	);

	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|UI", meta=(DevelopmentOnly))
	static void OpenModDirectoryForListItem(
		UWhiskerwoodModListItem* ModItem,
		bool bCreateIfMissing
	);

	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|UI", meta=(DevelopmentOnly))
	static void OpenModDirectoryForSettings(
		UPAL_WhiskerwoodModSettings* ModSettings,
		bool bCreateIfMissing
	);
};
