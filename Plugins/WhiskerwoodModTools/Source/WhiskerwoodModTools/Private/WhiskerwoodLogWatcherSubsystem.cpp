#include "WhiskerwoodLogWatcherSubsystem.h"

#include "WhiskerwoodEditorUserSettings.h"

#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "Containers/Ticker.h"


UWhiskerwoodEditorUserSettings* UWhiskerwoodLogWatcherSubsystem::GetEditorSettings() const
{
	return GetMutableDefault<UWhiskerwoodEditorUserSettings>();
}

void UWhiskerwoodLogWatcherSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	TimeSinceLastScan = 0.0f;
	ScanIntervalSeconds = 1.0f;

	// Build initial runtime state from settings
	RefreshFromSettings();

	// Register ticker
	TickerHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateUObject(this, &UWhiskerwoodLogWatcherSubsystem::HandleTicker),
		ScanIntervalSeconds
	);
}

void UWhiskerwoodLogWatcherSubsystem::Deinitialize()
{
	if (TickerHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
		// No need to manually reset; handle is now invalid after removal.
	}

	RuntimeStates.Reset();

	Super::Deinitialize();
}

bool UWhiskerwoodLogWatcherSubsystem::HandleTicker(float DeltaTime)
{
	TimeSinceLastScan += DeltaTime;

	if (TimeSinceLastScan >= ScanIntervalSeconds)
	{
		TimeSinceLastScan = 0.0f;
		ScanOnce();
	}

	// Returning true keeps the ticker active.
	return true;
}

void UWhiskerwoodLogWatcherSubsystem::RefreshFromSettings()
{
	RuntimeStates.Reset();

	if (UWhiskerwoodEditorUserSettings* Settings = GetEditorSettings())
	{
		for (const FWhiskerwoodLogWatchConfig& Config : Settings->LogWatchConfigs)
		{
			if (Config.Id.IsNone())
			{
				continue;
			}

			FWhiskerwoodLogRuntimeState& State = RuntimeStates.FindOrAdd(Config.Id);
			State.LastLineCount = 0;
			State.bHadFile = false;
		}
	}
}

void UWhiskerwoodLogWatcherSubsystem::SetConfigEnabled(FName ConfigId, bool bEnabled)
{
	if (ConfigId.IsNone())
	{
		return;
	}

	if (UWhiskerwoodEditorUserSettings* Settings = GetEditorSettings())
	{
		bool bChanged = false;

		for (FWhiskerwoodLogWatchConfig& Config : Settings->LogWatchConfigs)
		{
			if (Config.Id == ConfigId)
			{
				if (Config.bEnabled != bEnabled)
				{
					Config.bEnabled = bEnabled;
					bChanged = true;
				}
				break;
			}
		}

		if (bChanged)
		{
			Settings->SaveSettings();
			RefreshFromSettings();
		}
	}
}

void UWhiskerwoodLogWatcherSubsystem::ScanOnce()
{
	UWhiskerwoodEditorUserSettings* Settings = GetEditorSettings();
	if (!Settings)
	{
		return;
	}

	IFileManager& FileManager = IFileManager::Get();

	for (const FWhiskerwoodLogWatchConfig& Config : Settings->LogWatchConfigs)
	{
		if (!Config.bEnabled || Config.Id.IsNone())
		{
			continue;
		}

		const FString& FilePath = Config.LogFilePath;
		if (FilePath.IsEmpty())
		{
			continue;
		}

		FWhiskerwoodLogRuntimeState* StatePtr = RuntimeStates.Find(Config.Id);
		if (!StatePtr)
		{
			// Should have been created in RefreshFromSettings, but guard anyway.
			StatePtr = &RuntimeStates.Add(Config.Id);
			StatePtr->LastLineCount = 0;
			StatePtr->bHadFile = false;
		}

		FWhiskerwoodLogRuntimeState& State = *StatePtr;

		// Check if file exists
		if (!FileManager.FileExists(*FilePath))
		{
			// Reset state if the file disappeared
			State.LastLineCount = 0;
			State.bHadFile = false;
			continue;
		}

		// Load whole file as text (simple, not the most efficient, but fine for modest logs)
		FString FileContents;
		if (!FFileHelper::LoadFileToString(FileContents, *FilePath))
		{
			continue;
		}

		TArray<FString> Lines;
		FileContents.ParseIntoArrayLines(Lines);

		const int32 NewLineCount = Lines.Num();

		// First time we see this file, just set baseline line count and do not emit
		if (!State.bHadFile)
		{
			State.bHadFile = true;
			State.LastLineCount = NewLineCount;
			continue;
		}

		if (NewLineCount <= State.LastLineCount)
		{
			// No new lines (or file shrunk/rotated; in that case, reset)
			if (NewLineCount < State.LastLineCount)
			{
				State.LastLineCount = NewLineCount;
			}
			continue;
		}

		// Gather only the new lines since last time
		TArray<FString> NewLines;
		for (int32 Index = State.LastLineCount; Index < NewLineCount; ++Index)
		{
			NewLines.Add(Lines[Index]);
		}

		State.LastLineCount = NewLineCount;

		if (NewLines.Num() > 0)
		{
			OnLogLines.Broadcast(
				Config.Id,
				Config.DisplayName,
				FilePath,
				NewLines
			);
		}
	}
}
