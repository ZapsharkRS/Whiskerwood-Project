#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "WhiskerwoodModListRowWidget.generated.h"

class UTextBlock;
class UButton;
class UWhiskerwoodModListItem;
class UPAL_WhiskerwoodModSettings;

/**
 * Editor-only row widget for the Mod List.
 * 
 * This is used as the base class for WBP_ModListRow.
 * It:
 *  - Receives a UWhiskerwoodModListItem from the ListView
 *  - Calls the UI helper to refresh text/status/buttons
 *  - Wires Move/Remove/Deploy/OpenFolder buttons to C++ helpers
 */
UCLASS()
class WHISKERWOODMODTOOLS_API UWhiskerwoodModListRowWidget
	: public UUserWidget
	, public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	/** The backing data for this row (list item object). */
	UPROPERTY(BlueprintReadOnly, Category="Whiskerwood|ModUtils")
	UWhiskerwoodModListItem* ModItem = nullptr;

	// These will be bound from the BP (WBP_ModListRow)
	UPROPERTY(meta=(BindWidgetOptional))
	UTextBlock* NameText;

	UPROPERTY(meta=(BindWidgetOptional))
	UTextBlock* ChunkIdText;

	UPROPERTY(meta=(BindWidgetOptional))
	UTextBlock* StatusText;

	UPROPERTY(meta=(BindWidgetOptional))
	UButton* MoveButton;

	UPROPERTY(meta=(BindWidgetOptional))
	UButton* RemoveButton;

	UPROPERTY(meta=(BindWidgetOptional))
	UButton* DeployButton;

	UPROPERTY(meta=(BindWidgetOptional))
	UButton* OpenFolderButton;

public:
	// IUserObjectListEntry implementation
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

protected:
	// Widget lifecycle

	void BindButtonsIfNeeded();
	// Button handlers
	UFUNCTION()
	void HandleMoveClicked();

	UFUNCTION()
	void HandleRemoveClicked();

	UFUNCTION()
	void HandleDeployClicked();

	UFUNCTION()
	void HandleOpenFolderClicked();

public:
	/** Public helper: refresh UI state from ModItem. */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|UI")
	void RefreshRow();

private:
	bool bButtonsBound = false;
};
