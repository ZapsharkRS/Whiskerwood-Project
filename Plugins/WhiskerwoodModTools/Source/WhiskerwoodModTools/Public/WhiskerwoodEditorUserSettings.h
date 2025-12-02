// WhiskerwoodEditorUserSettings.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WhiskerwoodEditorUserSettings.generated.h"

/**
 * Per-user editor settings for Whiskerwood Mod Tools.
 *
 * Stored in EditorPerProjectUserSettings.ini
 * (so each dev / machine can have their own paths).
 */

USTRUCT(BlueprintType)
struct WHISKERWOODMODTOOLS_API FWhiskerwoodLogWatchConfig
{
	GENERATED_BODY()

public:

	/** Stable identifier for this watched log, used as a key. */
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Logs")
	FName Id;

	/** Friendly name for the UI. */
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Logs")
	FString DisplayName;

	/** Full path to the log file on disk. */
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Logs")
	FString LogFilePath;

	/** Whether this entry is currently enabled for watching. */
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Logs")
	bool bEnabled;

	FWhiskerwoodLogWatchConfig()
		: Id(NAME_None)
		, bEnabled(true)
	{
	}
};

UCLASS(Config=EditorPerProjectUserSettings, BlueprintType)
class WHISKERWOODMODTOOLS_API UWhiskerwoodEditorUserSettings : public UObject
{
	GENERATED_BODY()

public:

	UWhiskerwoodEditorUserSettings(const FObjectInitializer& ObjectInitializer);

	// --------------------------------------------------------------------
	// Core Paths
	// --------------------------------------------------------------------

	/**
	 * Mods Directory - Directory for the game itself where Saved/mods is.
	 *
	 * Example:
	 *   C:/Users/<User>/AppData/Local/Whiskerwood/Saved/mods
	 *
	 * If empty, tools will fall back to their own auto-detection based
	 * on LOCALAPPDATA and WhiskerwoodFileIOLibrary.
	 */
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Paths")
	FString ModsDirectory;

	/**
	 * Project Directory - Project root for this Whiskerwood SDK project.
	 *
	 * Example:
	 *   D:/WW/WhiskerwoodProject/Whiskerwood-Project/
	 */
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Paths")
	FString ProjectDirectory;

	/**
	 * Pak Directory - Directory where pakchunk*.pak files are generated.
	 *
	 * Sometimes this is under:
	 *   <Project>/Saved/StagedBuilds/WindowsNoEditor/Whiskerwood/Content/Paks
	 * or another platform-specific location.
	 *
	 * If empty, tools are allowed to recursively search from ProjectDirectory
	 * to find pakchunk<ChunkID>-*.pak files.
	 */
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Paths")
	FString PakDirectory;

	/**
	 * App Data Directory - Root of Whiskerwood's AppData directory.
	 *
	 * Example:
	 *   C:/Users/<User>/AppData/Local/Whiskerwood
	 *
	 * From this, we can derive:
	 *   Saved/mods
	 *   Saved/Logs
	 *   Workshop/TempDeploy/...
	 */
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Paths")
	FString AppDataDirectory;

	/**
	 * Platform name - human-readable platform, e.g. "Windows", "Linux".
	 *
	 * This is used both:
	 *  - To influence where we look for pak output
	 *  - To map to the UAT platform name (Win64, Linux, etc.).
	 */
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Platform")
	FString PlatformName;

	// --------------------------------------------------------------------
	// Steam / Workshop
	// --------------------------------------------------------------------

	/**
	 * Steam App ID - ID for Whiskerwood on Steam.
	 *
	 * Should default to 2489330.
	 */
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Steam")
	int32 SteamAppID;

	/**
	 * Temporary Deploy Directory - base directory used to stage mods
	 * for Steam Workshop packaging.
	 *
	 * Example default:
	 *   C:/Users/<User>/AppData/Local/Whiskerwood/WorkshopTemp
	 *
	 * The mod tools will create subfolders per mod inside this directory
	 * when preparing workshop uploads.
	 */
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Steam")
	FString TempDeployDirectory;

	// --------------------------------------------------------------------
	// Logs / Diagnostics
	// --------------------------------------------------------------------

	/**
	 * Base Logs Directory - game's log directory under AppData.
	 *
	 * Example:
	 *   C:/Users/<User>/AppData/Local/Whiskerwood/Saved/Logs
	 *
	 * This is useful both for watching the live game logs and for
	 * utilities that dump datatables to JSON into this area.
	 */
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Logs")
	FString BaseLogsDirectory;

	/**
	 * List of log files the editor subsystem should watch.
	 *
	 * This is per-user, per-project configuration.
	 *
	 * NOTE: In the future, the JSON dump viewer panel can use
	 * BaseLogsDirectory and this list to discover and display
	 * datatable JSON dumps.
	 */
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Logs")
	TArray<FWhiskerwoodLogWatchConfig> LogWatchConfigs;

	// --------------------------------------------------------------------
	// Misc / Utilities
	// --------------------------------------------------------------------

	/** Last copied .pak file path (for quick re-use / debugging). */
	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Misc")
	FString LastCopiedPakPath;

public:

	/** Writes UPROPERTY(Config) values to EditorPerProjectUserSettings.ini */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|Settings", meta=(DevelopmentOnly))
	void SaveSettings();

	/** Gets the singleton settings object instance */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|Settings", meta=(DevelopmentOnly))
	static UWhiskerwoodEditorUserSettings* GetSettings();
};
