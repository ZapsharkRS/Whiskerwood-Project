// Public/WhiskerwoodModTools.h

#pragma once

#include "Modules/ModuleManager.h"
#include "Delegates/Delegate.h" // For FDelegateHandle, FSimpleDelegate

class UEditorUtilityWidgetBlueprint;

/**
 * Main editor module for Whiskerwood Mod Tools.
 *
 * Responsibilities:
 * - Register a toolbar button in the Level Editor (near the Play controls).
 * - When clicked, open the EUW_WWModToolsEditorWidget as a tab.
 */
class FWhiskerwoodModToolsModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Handler for clicking the toolbar button (must be public for CreateRaw). */
	void OnOpenModToolsDashboardClicked();

private:

	/** Called by UToolMenus once it is ready; we register our toolbar button here. */
	void RegisterMenus();

	/** Helper: Load the EUW BP for the dashboard. */
	UEditorUtilityWidgetBlueprint* LoadModToolsEditorWidget() const;

private:

	/** Handle to the ToolMenus startup callback so we can unregister it on shutdown. */
	FDelegateHandle ToolMenusStartupHandle;
};
