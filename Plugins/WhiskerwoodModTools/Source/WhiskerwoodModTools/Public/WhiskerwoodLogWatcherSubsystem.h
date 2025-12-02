#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "Containers/Ticker.h" // For FTSTicker::FDelegateHandle

#include "WhiskerwoodLogWatcherSubsystem.generated.h"

class UWhiskerwoodEditorUserSettings;

/**
 * Called whenever new lines are detected in a watched log file.
 *
 * @param ConfigId    The Id from FWhiskerwoodLogWatchConfig.
 * @param DisplayName Friendly name from the config.
 * @param FilePath    Full path to the log file.
 * @param NewLines    All new lines since last scan.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FWhiskerwoodOnLogLines,
	FName, ConfigId,
	const FString&, DisplayName,
	const FString&, FilePath,
	const TArray<FString>&, NewLines
);

/** Internal runtime state for a given watched log. */
struct FWhiskerwoodLogRuntimeState
{
	int32 LastLineCount = 0;
	bool bHadFile = false;
};

/**
 * Editor subsystem that watches configured log files and
 * broadcasts new lines to interested widgets / tools.
 */
UCLASS()
class WHISKERWOODMODTOOLS_API UWhiskerwoodLogWatcherSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:

	// UEditorSubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Rebuilds runtime state from UWhiskerwoodEditorUserSettings::LogWatchConfigs.
	 * Call this after modifying settings via BP.
	 */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|Subsystems|LogWatcher", meta=(DevelopmentOnly))
	void RefreshFromSettings();

	/**
	 * Convenience BP helper to toggle a given config entry.
	 * This also calls SaveSettings() and RefreshFromSettings().
	 */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|Subsystems|LogWatcher", meta=(DevelopmentOnly))
	void SetConfigEnabled(FName ConfigId, bool bEnabled);

	/** Delegate fired when new lines are observed in a watched log. */
	UPROPERTY(BlueprintAssignable, Category="Whiskerwood|ModUtils|Subsystems|LogWatcher")
	FWhiskerwoodOnLogLines OnLogLines;

private:

	/** Ticker callback that periodically polls log files. */
	bool HandleTicker(float DeltaTime);

	/** Performs a single scan over all configured log files. */
	void ScanOnce();

	/** Helper to access the settings object. */
	UWhiskerwoodEditorUserSettings* GetEditorSettings() const;

private:

	/** Map from config Id -> runtime line-count state. */
	TMap<FName, FWhiskerwoodLogRuntimeState> RuntimeStates;

	/** Handle to the ticker registration. */
	FTSTicker::FDelegateHandle TickerHandle;

	/** Seconds between scans; tweak as desired. */
	float ScanIntervalSeconds = 1.0f;

	/** Accumulated time since last scan. */
	float TimeSinceLastScan = 0.0f;
};
