#include "WhiskerwoodModToolsUILibrary.h"

#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"

#include "WhiskerwoodModListItem.h"
#include "WhiskerwoodModToolsLibrary.h"
#include "WhiskerwoodEditorUserSettings.h"
#include "PAL_WhiskerwoodModSettings.h"

#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "HAL/PlatformFile.h"

#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "GenericPlatform/GenericApplication.h"

namespace
{
	// Simple green/red colors for label status.
	const FLinearColor ColorGreen(0.f, 1.f, 0.f, 1.f);
	const FLinearColor ColorRed  (1.f, 0.f, 0.f, 1.f);

	// Status colors for the mod row
	const FLinearColor StatusOk    (0.4f, 1.f,   0.4f, 1.f);
	const FLinearColor StatusWarn  (1.f,   1.f,  0.3f, 1.f);
	const FLinearColor StatusError (1.f,   0.4f, 0.4f, 1.f);

	void ApplyLabelColor(UTextBlock* Label, bool bOk)
	{
		if (!Label)
		{
			return;
		}

		const FLinearColor Color = bOk ? ColorGreen : ColorRed;
		Label->SetColorAndOpacity(FSlateColor(Color));
	}

	void EnsureTextBoxValue(UEditableTextBox* TextBox, const FString& Value)
	{
		if (!TextBox)
		{
			return;
		}

		// Only fill if currently empty
		if (TextBox->GetText().IsEmpty())
		{
			TextBox->SetText(FText::FromString(Value));
		}
	}

	FString BuildModDirectoryFromSettings(UPAL_WhiskerwoodModSettings* ModSettings)
	{
		if (!ModSettings)
		{
			return FString();
		}

		// Root mods dir (Saved/mods) resolved from settings + auto-detect.
		const FString BaseModsDir = UWhiskerwoodModToolsLibrary::ResolveModsDirectory();

		FString DirName;

#if WITH_EDITORONLY_DATA
		DirName = ModSettings->ModDirName;
#endif

		if (DirName.IsEmpty())
		{
			// Fallback: asset name
			DirName = ModSettings->GetName();
		}

		if (DirName.IsEmpty())
		{
			return FString();
		}

		FString FullPath = FPaths::Combine(BaseModsDir, DirName);
		FPaths::NormalizeDirectoryName(FullPath);

		return FullPath;
	}
}

// ---------------------------------------------------------------------
// Mod directory open helpers
// ---------------------------------------------------------------------

void UWhiskerwoodModToolsUILibrary::OpenModDirectoryForSettings(
	UPAL_WhiskerwoodModSettings* ModSettings,
	bool bCreateIfMissing)
{
#if WITH_EDITOR
	if (!ModSettings)
	{
		return;
	}

	const FString ModDir = BuildModDirectoryFromSettings(ModSettings);
	if (ModDir.IsEmpty())
	{
		return;
	}

	IPlatformFile& PF = FPlatformFileManager::Get().GetPlatformFile();

	if (!PF.DirectoryExists(*ModDir))
	{
		if (bCreateIfMissing)
		{
			if (!PF.CreateDirectoryTree(*ModDir))
			{
				return; // failed to create
			}
		}
		else
		{
			return;
		}
	}

	FPlatformProcess::ExploreFolder(*ModDir);
#endif // WITH_EDITOR
}

void UWhiskerwoodModToolsUILibrary::OpenModDirectoryForListItem(
	UWhiskerwoodModListItem* ModItem,
	bool bCreateIfMissing)
{
#if WITH_EDITOR
	if (!ModItem)
	{
		return;
	}

	UPAL_WhiskerwoodModSettings* ModSettings = ModItem->ModSettings;
	if (!ModSettings)
	{
		return;
	}

	OpenModDirectoryForSettings(ModSettings, bCreateIfMissing);
#endif // WITH_EDITOR
}

// ---------------------------------------------------------------------
// Refresh Settings UI
// ---------------------------------------------------------------------

void UWhiskerwoodModToolsUILibrary::RefreshSettingsBasicUI(
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
	UEditableTextBox* TempDeployTextBox)
{
	// Grab settings (for PlatformName + SteamAppID defaults)
	const UWhiskerwoodEditorUserSettings* Settings =
		GetDefault<UWhiskerwoodEditorUserSettings>();

	// -----------------------
	// PROJECT DIRECTORY
	// -----------------------

	const FString ResolvedProject = UWhiskerwoodModToolsLibrary::ResolveProjectDirectory();
	EnsureTextBoxValue(ProjectTextBox, ResolvedProject);

	FString EffectiveProject = ResolvedProject;
	if (ProjectTextBox)
	{
		const FString UserValue = ProjectTextBox->GetText().ToString();
		if (!UserValue.IsEmpty())
		{
			EffectiveProject = UserValue;
		}
	}

	const bool bProjectExists =
		UWhiskerwoodModToolsLibrary::DoesDirectoryExist(EffectiveProject);

	ApplyLabelColor(ProjectLabel, bProjectExists);

	// -----------------------
	// APPDATA DIRECTORY
	// -----------------------

	const FString ResolvedAppData = UWhiskerwoodModToolsLibrary::ResolveAppDataDirectory();
	EnsureTextBoxValue(AppDataTextBox, ResolvedAppData);

	FString EffectiveAppData = ResolvedAppData;
	if (AppDataTextBox)
	{
		const FString UserValue = AppDataTextBox->GetText().ToString();
		if (!UserValue.IsEmpty())
		{
			EffectiveAppData = UserValue;
		}
	}

	const bool bAppDataExists =
		UWhiskerwoodModToolsLibrary::DoesDirectoryExist(EffectiveAppData);

	ApplyLabelColor(AppDataLabel, bAppDataExists);

	// -----------------------
	// MODS DIRECTORY
	// -----------------------

	const FString ResolvedMods = UWhiskerwoodModToolsLibrary::ResolveModsDirectory();
	EnsureTextBoxValue(ModsTextBox, ResolvedMods);

	FString EffectiveMods = ResolvedMods;
	if (ModsTextBox)
	{
		const FString UserValue = ModsTextBox->GetText().ToString();
		if (!UserValue.IsEmpty())
		{
			EffectiveMods = UserValue;
		}
	}

	const bool bModsExists =
		UWhiskerwoodModToolsLibrary::DoesDirectoryExist(EffectiveMods);

	ApplyLabelColor(ModsLabel, bModsExists);

	// -----------------------
	// PLATFORM
	// -----------------------

	FString HumanPlatform = TEXT("Windows");
	if (Settings && !Settings->PlatformName.IsEmpty())
	{
		HumanPlatform = Settings->PlatformName;
	}

	// If the text box is empty, fill with the human platform name
	if (PlatformTextBox)
	{
		if (PlatformTextBox->GetText().IsEmpty())
		{
			PlatformTextBox->SetText(FText::FromString(HumanPlatform));
		}
		else
		{
			HumanPlatform = PlatformTextBox->GetText().ToString();
		}
	}

	// Map to UAT platform; if we get something non-empty back, call it valid
	const FString UATPlatform =
		UWhiskerwoodModToolsLibrary::ResolveUATPlatformName();

	const bool bPlatformOk = !UATPlatform.IsEmpty();
	ApplyLabelColor(PlatformLabel, bPlatformOk);

	// -----------------------
	// STEAM APP ID
	// -----------------------

	int32 SteamAppID = 0;
	if (Settings)
	{
		SteamAppID = Settings->SteamAppID;
	}

	// Fill text box if empty
	if (SteamAppIDTextBox)
	{
		if (SteamAppIDTextBox->GetText().IsEmpty())
		{
			const FString SteamStr = FString::FromInt(SteamAppID);
			SteamAppIDTextBox->SetText(FText::FromString(SteamStr));
		}

		// Re-parse from whatever is in the box now
		const FString UserSteamStr = SteamAppIDTextBox->GetText().ToString();
		if (!UserSteamStr.IsEmpty())
		{
			SteamAppID = FCString::Atoi(*UserSteamStr);
		}
	}

	const bool bSteamOk = (SteamAppID > 0);
	ApplyLabelColor(SteamAppIDLabel, bSteamOk);

	// -----------------------
	// TEMP DEPLOY DIRECTORY
	// -----------------------

	const FString ResolvedTemp =
		UWhiskerwoodModToolsLibrary::ResolveTempDeployDirectory();
	EnsureTextBoxValue(TempDeployTextBox, ResolvedTemp);

	FString EffectiveTemp = ResolvedTemp;
	if (TempDeployTextBox)
	{
		const FString UserValue = TempDeployTextBox->GetText().ToString();
		if (!UserValue.IsEmpty())
		{
			EffectiveTemp = UserValue;
		}
	}

	const bool bTempExists =
		UWhiskerwoodModToolsLibrary::DoesDirectoryExist(EffectiveTemp);

	ApplyLabelColor(TempDeployLabel, bTempExists);
}

// ---------------------------------------------------------------------
// Save Settings from UI
// ---------------------------------------------------------------------

void UWhiskerwoodModToolsUILibrary::SaveSettingsFromBasicUI(
	UEditableTextBox* ProjectTextBox,
	UEditableTextBox* AppDataTextBox,
	UEditableTextBox* ModsTextBox,
	UEditableTextBox* PlatformTextBox,
	UEditableTextBox* SteamAppIDTextBox,
	UEditableTextBox* TempDeployTextBox)
{
	UWhiskerwoodEditorUserSettings* Settings =
		GetMutableDefault<UWhiskerwoodEditorUserSettings>();

	if (!Settings)
	{
		return;
	}

	// Project Directory
	if (ProjectTextBox)
	{
		Settings->ProjectDirectory = ProjectTextBox->GetText().ToString();
	}

	// AppData Directory
	if (AppDataTextBox)
	{
		Settings->AppDataDirectory = AppDataTextBox->GetText().ToString();
	}

	// Mods Directory
	if (ModsTextBox)
	{
		Settings->ModsDirectory = ModsTextBox->GetText().ToString();
	}

	// Platform Name
	if (PlatformTextBox)
	{
		Settings->PlatformName = PlatformTextBox->GetText().ToString();
	}

	// Steam App ID
	if (SteamAppIDTextBox)
	{
		const FString SteamStr = SteamAppIDTextBox->GetText().ToString();
		Settings->SteamAppID = SteamStr.IsEmpty() ? 0 : FCString::Atoi(*SteamStr);
	}

	// Temp Deploy Directory
	if (TempDeployTextBox)
	{
		Settings->TempDeployDirectory = TempDeployTextBox->GetText().ToString();
	}

	// Persist to EditorPerProjectUserSettings.ini
	Settings->SaveSettings();
}

// ---------------------------------------------------------------------
// Optional: SettingsPaths UI
// ---------------------------------------------------------------------

void UWhiskerwoodModToolsUILibrary::RefreshSettingsPathsUI(
	UTextBlock* ProjectLabel,
	UEditableTextBox* ProjectTextBox,
	UTextBlock* ModsLabel,
	UEditableTextBox* ModsTextBox,
	UTextBlock* AppDataLabel,
	UEditableTextBox* AppDataTextBox,
	UTextBlock* LogsLabel,
	UEditableTextBox* LogsTextBox,
	UTextBlock* TempDeployLabel,
	UEditableTextBox* TempDeployTextBox,
	UTextBlock* PakLabel,
	UEditableTextBox* PakTextBox)
{
	// Basic reuse: call the simpler helper for Project/AppData/Mods/Platform/Steam/Temp
	RefreshSettingsBasicUI(
		ProjectLabel,
		ProjectTextBox,
		AppDataLabel,
		AppDataTextBox,
		ModsLabel,
		ModsTextBox,
		/*PlatformLabel*/ nullptr,
		/*PlatformTextBox*/ nullptr,
		/*SteamAppIDLabel*/ nullptr,
		/*SteamAppIDTextBox*/ nullptr,
		TempDeployLabel,
		TempDeployTextBox
	);

	// Logs
	const FString LogsDir = UWhiskerwoodModToolsLibrary::ResolveBaseLogsDirectory();
	EnsureTextBoxValue(LogsTextBox, LogsDir);
	const bool bLogsExists = UWhiskerwoodModToolsLibrary::DoesDirectoryExist(
		LogsTextBox ? LogsTextBox->GetText().ToString() : LogsDir);
	ApplyLabelColor(LogsLabel, bLogsExists);

	// Pak
	const FString PakDir = UWhiskerwoodModToolsLibrary::ResolvePakDirectory();
	EnsureTextBoxValue(PakTextBox, PakDir);
	const bool bPakExists = UWhiskerwoodModToolsLibrary::DoesDirectoryExist(
		PakTextBox ? PakTextBox->GetText().ToString() : PakDir);
	ApplyLabelColor(PakLabel, bPakExists);
}

// ---------------------------------------------------------------------
// Set Temp Deploy to TempWorkshop default
// ---------------------------------------------------------------------

void UWhiskerwoodModToolsUILibrary::SetTempDeployToWorkshopDefault(
	UTextBlock* TempDeployLabel,
	UEditableTextBox* TempDeployTextBox)
{
	// Base appdata dir (resolved from settings + autodetect)
	const FString AppDataDir = UWhiskerwoodModToolsLibrary::ResolveAppDataDirectory();

	// Use folder name "TempWorkshop" under AppData root
	FString TempWorkshopDir = FPaths::Combine(AppDataDir, TEXT("TempWorkshop"));
	FPaths::NormalizeDirectoryName(TempWorkshopDir);

	// Create directory tree
	IPlatformFile& PF = FPlatformFileManager::Get().GetPlatformFile();
	const bool bCreatedOrExists = PF.CreateDirectoryTree(*TempWorkshopDir);

	// Update textbox
	if (TempDeployTextBox)
	{
		TempDeployTextBox->SetText(FText::FromString(TempWorkshopDir));
	}

	// Update settings + save
	if (UWhiskerwoodEditorUserSettings* Settings = GetMutableDefault<UWhiskerwoodEditorUserSettings>())
	{
		Settings->TempDeployDirectory = TempWorkshopDir;
		Settings->SaveSettings();
	}

	// Color label based on success
	ApplyLabelColor(TempDeployLabel, bCreatedOrExists);
}

// ---------------------------------------------------------------------
// Generic open/browse helpers
// ---------------------------------------------------------------------

void UWhiskerwoodModToolsUILibrary::OpenDirectoryFromTextBox(
	UEditableTextBox* SourceTextBox,
	bool bCreateIfMissing)
{
	if (!SourceTextBox)
	{
		return;
	}

	FString DirPath = SourceTextBox->GetText().ToString().TrimStartAndEnd();

	// If empty, fall back to something sensible so the button isn't useless.
	if (DirPath.IsEmpty())
	{
		// Default: resolved project directory
		DirPath = UWhiskerwoodModToolsLibrary::ResolveProjectDirectory();
	}

	if (DirPath.IsEmpty())
	{
		return;
	}

	IPlatformFile& PF = FPlatformFileManager::Get().GetPlatformFile();

	if (!PF.DirectoryExists(*DirPath))
	{
		if (bCreateIfMissing)
		{
			if (!PF.CreateDirectoryTree(*DirPath))
			{
				// Could log here if you want
				return;
			}
		}
		else
		{
			return;
		}
	}

	// At this point, DirPath exists – open in Explorer/Finder/etc.
	FPlatformProcess::ExploreFolder(*DirPath);
}

void UWhiskerwoodModToolsUILibrary::BrowseForDirectoryIntoTextBox(
	UEditableTextBox* TargetTextBox,
	FText DialogTitle)
{
	if (!TargetTextBox)
	{
		return;
	}

	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
	{
		return;
	}

	// Determine a parent window handle for the dialog (optional but nicer).
	void* ParentWindowHandle = nullptr;

	if (FSlateApplication::IsInitialized())
	{
		TSharedPtr<SWindow> ParentWindow =
			FSlateApplication::Get().FindBestParentWindowForDialogs(nullptr);

		if (ParentWindow.IsValid() && ParentWindow->GetNativeWindow().IsValid())
		{
			ParentWindowHandle = ParentWindow->GetNativeWindow()->GetOSWindowHandle();
		}
	}

	// Use current textbox content as starting folder, or project dir as fallback.
	FString DefaultPath = TargetTextBox->GetText().ToString().TrimStartAndEnd();
	if (DefaultPath.IsEmpty())
	{
		DefaultPath = UWhiskerwoodModToolsLibrary::ResolveProjectDirectory();
	}

	FString ChosenDir;
	const bool bPicked = DesktopPlatform->OpenDirectoryDialog(
		ParentWindowHandle,
		DialogTitle.ToString(),
		DefaultPath,
		ChosenDir
	);

	if (!bPicked || ChosenDir.IsEmpty())
	{
		return;
	}

	// Normalize and assign to textbox
	FPaths::NormalizeDirectoryName(ChosenDir);
	TargetTextBox->SetText(FText::FromString(ChosenDir));
}

// ---------------------------------------------------------------------
// Open special dirs (Mods / Logs / TempWorkshop)
// ---------------------------------------------------------------------

void UWhiskerwoodModToolsUILibrary::OpenWhiskerwoodSpecialDirectory(
	EWhiskerwoodSpecialDirectory DirectoryType,
	bool bCreateIfMissing)
{
#if WITH_EDITOR
	FString TargetDir;

	switch (DirectoryType)
	{
	case EWhiskerwoodSpecialDirectory::Mods:
		// e.g. C:/Users/<User>/AppData/Local/Whiskerwood/Saved/mods
		TargetDir = UWhiskerwoodModToolsLibrary::ResolveModsDirectory();
		break;

	case EWhiskerwoodSpecialDirectory::Logs:
		// AppData/Local/Whiskerwood/Saved/Logs
		TargetDir = UWhiskerwoodModToolsLibrary::ResolveBaseLogsDirectory();
		break;

	case EWhiskerwoodSpecialDirectory::TempWorkshop:
	default:
		// Use your Temp Deploy / Temp Workshop directory
		TargetDir = UWhiskerwoodModToolsLibrary::ResolveTempDeployDirectory();
		break;
	}

	if (TargetDir.IsEmpty())
	{
		return;
	}

	IPlatformFile& PF = FPlatformFileManager::Get().GetPlatformFile();

	if (!PF.DirectoryExists(*TargetDir))
	{
		if (bCreateIfMissing)
		{
			if (!PF.CreateDirectoryTree(*TargetDir))
			{
				return; // failed to create
			}
		}
		else
		{
			return; // don't create, don't open
		}
	}

	FPlatformProcess::ExploreFolder(*TargetDir);
#endif // WITH_EDITOR
}

// ---------------------------------------------------------------------
// MOD LIST ROW UI
// ---------------------------------------------------------------------

void UWhiskerwoodModToolsUILibrary::RefreshModListRowUI(
	UWhiskerwoodModListItem* ModItem,
	UTextBlock* NameText,
	UTextBlock* ChunkIdText,
	UTextBlock* StatusText,
	UButton* MoveButton,
	UButton* RemoveButton,
	UButton* DeployButton)
{
	// Default: everything disabled / blank
	auto SetButtonEnabled = [](UButton* Button, bool bEnabled)
	{
		if (Button)
		{
			Button->SetIsEnabled(bEnabled);
		}
	};

	// Safe defaults
	SetButtonEnabled(MoveButton, false);
	SetButtonEnabled(RemoveButton, false);
	SetButtonEnabled(DeployButton, false);

	if (NameText)
	{
		NameText->SetText(FText::GetEmpty());
	}
	if (ChunkIdText)
	{
		ChunkIdText->SetText(FText::GetEmpty());
	}
	if (StatusText)
	{
		StatusText->SetText(FText::FromString(TEXT("No Mod")));
		StatusText->SetColorAndOpacity(FSlateColor(StatusError));
	}

	if (!ModItem || !ModItem->ModSettings)
	{
		// Nothing more to do
		return;
	}

	UPAL_WhiskerwoodModSettings* ModSettings = ModItem->ModSettings;

	// --------------------------
	// Name & ChunkID display
	// --------------------------

	FString DisplayName = ModItem->ModName;
	if (DisplayName.IsEmpty())
	{
#if WITH_EDITORONLY_DATA
		if (!ModSettings->ModName.IsEmpty())
		{
			DisplayName = ModSettings->ModName;
		}
		else
#endif
		{
			DisplayName = ModSettings->GetName();
		}
	}

	if (NameText)
	{
		NameText->SetText(FText::FromString(DisplayName));
	}

	int32 ChunkIdValue = 0;
#if WITH_EDITORONLY_DATA
	ChunkIdValue = ModSettings->GetChunkId();
#endif

	if (ChunkIdText)
	{
		if (ChunkIdValue > 0)
		{
			ChunkIdText->SetText(FText::AsNumber(ChunkIdValue));
		}
		else
		{
			ChunkIdText->SetText(FText::FromString(TEXT("-")));
		}
	}

	// --------------------------
	// Status calculation
	// --------------------------

	const bool bHasSourcePak    = UWhiskerwoodModToolsLibrary::HasSourcePak(ModSettings);
	const bool bIsMoved      = UWhiskerwoodModToolsLibrary::IsModMoved(ModSettings);
	const bool bCanDeploySteam  = UWhiskerwoodModToolsLibrary::CanDeployToWorkshop(ModSettings);

	FText Status = FText::GetEmpty();
	FLinearColor StatusColor = StatusWarn;

	if (ChunkIdValue <= 0)
	{
		Status      = FText::FromString(TEXT("Missing ChunkID"));
		StatusColor = StatusError;
	}
	else if (!bIsMoved && !bHasSourcePak)
	{
		// Nothing built and nothing moved yet
		Status      = FText::FromString(TEXT("No Pak Found"));
		StatusColor = StatusError;
	}
	else if (!bIsMoved && bHasSourcePak)
	{
		// Pak exists in project, but hasn't been copied into the mods folder
		Status      = FText::FromString(TEXT("Ready to be Moved"));
		StatusColor = StatusOk;
	}
	else if (bIsMoved)
	{
		// Mod has been moved into Saved/mods (regardless of source pak)
		if (bCanDeploySteam)
		{
			Status      = FText::FromString(TEXT("Moved | Ready for Deploy"));
			StatusColor = StatusOk;
		}
		else
		{
			Status      = FText::FromString(TEXT("Moved | Steam Not Configured"));
			StatusColor = StatusWarn;
		}
	}
	else
	{
		// Fallback - shouldn't normally hit
		Status      = FText::FromString(TEXT("Unknown State"));
		StatusColor = StatusWarn;
	}

	if (StatusText)
	{
		StatusText->SetText(Status);
		StatusText->SetColorAndOpacity(FSlateColor(StatusColor));
	}

	// --------------------------
	// Button enabling rules
	// --------------------------

	// Move -> only when there's a source pak and it hasn't been moved yet
	SetButtonEnabled(MoveButton,  bHasSourcePak && !bIsMoved && ChunkIdValue > 0);

	// Remove -> only if the mod is currently in Saved/mods
	SetButtonEnabled(RemoveButton, bIsMoved);

	// Deploy -> only if moved and Steam is configured
	SetButtonEnabled(DeployButton, bIsMoved && bCanDeploySteam);
}
