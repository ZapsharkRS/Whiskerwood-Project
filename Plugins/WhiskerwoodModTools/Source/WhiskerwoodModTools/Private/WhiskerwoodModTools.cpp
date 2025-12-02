// Private/WhiskerwoodModTools.cpp
// Whiskerwood Mod Tools - main editor module

#include "WhiskerwoodModTools.h"

#include "ToolMenus.h"
#include "WhiskerwoodModToolsStyle.h"

#if WITH_EDITOR
#include "Editor.h"
#include "LevelEditor.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#endif

#define LOCTEXT_NAMESPACE "FWhiskerwoodModToolsModule"

void FWhiskerwoodModToolsModule::StartupModule()
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Log, TEXT("[WhiskerwoodModTools] StartupModule"));

	// Initialize our icon style (loads Icon_WWModTools_40, etc.)
	FWhiskerwoodModToolsStyle::Initialize();

	// Ask ToolMenus to call us when it is ready so we can register our toolbar button.
	ToolMenusStartupHandle = UToolMenus::RegisterStartupCallback(
		FSimpleDelegate::CreateRaw(
			this,
			&FWhiskerwoodModToolsModule::RegisterMenus
		)
	);
#endif
}

void FWhiskerwoodModToolsModule::ShutdownModule()
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Log, TEXT("[WhiskerwoodModTools] ShutdownModule"));

	if (UToolMenus::IsToolMenuUIEnabled())
	{
		UToolMenus::UnRegisterStartupCallback(ToolMenusStartupHandle);
	}

	FWhiskerwoodModToolsStyle::Shutdown();
#endif
}

void FWhiskerwoodModToolsModule::RegisterMenus()
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Log, TEXT("[WhiskerwoodModTools] RegisterMenus called."));

	UToolMenus* ToolMenus = UToolMenus::Get();
	if (!ToolMenus)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WhiskerwoodModTools] UToolMenus::Get() returned nullptr."));
		return;
	}

	// Extend the Play toolbar so the button appears near the Play controls.
	UToolMenu* PlayToolbarMenu =
		ToolMenus->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");

	if (!PlayToolbarMenu)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[WhiskerwoodModTools] Failed to extend 'LevelEditor.LevelEditorToolBar.PlayToolBar'.")
		);
		return;
	}

	// Add our own section on that toolbar.
	FToolMenuSection& Section = PlayToolbarMenu->FindOrAddSection("WhiskerwoodModToolsSection");

	UE_LOG(LogTemp, Log, TEXT("[WhiskerwoodModTools] Adding toolbar button to Play toolbar."));

	FToolMenuEntry Entry = FToolMenuEntry::InitToolBarButton(
		FName("WhiskerwoodModTools_OpenDashboard"),
		FToolUIActionChoice(
			FUIAction(
				FExecuteAction::CreateRaw(
					this,
					&FWhiskerwoodModToolsModule::OnOpenModToolsDashboardClicked
				)
			)
		),
		LOCTEXT("WWModTools_ButtonLabel", "WW Mod Tools"),
		LOCTEXT("WWModTools_ButtonTooltip", "Open the Whiskerwood Mod Tools dashboard."),
		FSlateIcon(
			FWhiskerwoodModToolsStyle::GetStyleSetName(),
			TEXT("WhiskerwoodModTools.ToolbarIcon")
		),
		EUserInterfaceActionType::Button,
		NAME_None
	);

	Section.AddEntry(Entry);

	UE_LOG(LogTemp, Log, TEXT("[WhiskerwoodModTools] Toolbar button added to Play toolbar."));
#endif
}

void FWhiskerwoodModToolsModule::OnOpenModToolsDashboardClicked()
{
#if WITH_EDITOR
	UE_LOG(LogTemp, Log, TEXT("[WhiskerwoodModTools] Toolbar button clicked — opening EUW."));

	if (!GEditor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WhiskerwoodModTools] GEditor is null; cannot open EUW."));
		return;
	}

	// Load the EUW asset
	UEditorUtilityWidgetBlueprint* WidgetBP = LoadModToolsEditorWidget();
	if (!WidgetBP)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[WhiskerwoodModTools] Failed to load EUW_WWModToolsEditorWidget. Check asset path.")
		);
		return;
	}

	// Use the EditorUtilitySubsystem to spawn the tab
	if (UEditorUtilitySubsystem* EditorUtilitySubsystem =
		    GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>())
	{
		EditorUtilitySubsystem->SpawnAndRegisterTab(WidgetBP);
	}
	else
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[WhiskerwoodModTools] Could not get EditorUtilitySubsystem; cannot spawn tab.")
		);
	}
#endif
}

UEditorUtilityWidgetBlueprint* FWhiskerwoodModToolsModule::LoadModToolsEditorWidget() const
{
#if WITH_EDITOR
	// C++ only needs the object path, not the /Script prefix:
	// /Script/Blutility.EditorUtilityWidgetBlueprint'/WhiskerwoodModTools/EUW_WWModToolsEditorWidget.EUW_WWModToolsEditorWidget'
	// =>
	// /WhiskerwoodModTools/EUW_WWModToolsEditorWidget.EUW_WWModToolsEditorWidget

	const FString AssetPath =
		TEXT("/WhiskerwoodModTools/EUW_WWModToolsEditorWidget.EUW_WWModToolsEditorWidget");

	UObject* LoadedObj = StaticLoadObject(
		UEditorUtilityWidgetBlueprint::StaticClass(),
		/*Outer=*/nullptr,
		*AssetPath
	);

	return Cast<UEditorUtilityWidgetBlueprint>(LoadedObj);
#else
	return nullptr;
#endif
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FWhiskerwoodModToolsModule, WhiskerwoodModTools)
