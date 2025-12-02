#include "WhiskerwoodModListRowWidget.h"

#include "WhiskerwoodModListItem.h"
#include "PAL_WhiskerwoodModSettings.h"
#include "WhiskerwoodModToolsLibrary.h"
#include "WhiskerwoodModToolsUILibrary.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"

void UWhiskerwoodModListRowWidget::BindButtonsIfNeeded()
{
	if (bButtonsBound)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[%s] BindButtonsIfNeeded called."), *GetName());

	if (MoveButton)
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] MoveButton is valid, binding click."), *GetName());
		MoveButton->OnClicked.AddDynamic(this, &UWhiskerwoodModListRowWidget::HandleMoveClicked);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] MoveButton is NULL in BindButtonsIfNeeded."), *GetName());
	}

	if (RemoveButton)
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] RemoveButton is valid, binding click."), *GetName());
		RemoveButton->OnClicked.AddDynamic(this, &UWhiskerwoodModListRowWidget::HandleRemoveClicked);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] RemoveButton is NULL in BindButtonsIfNeeded."), *GetName());
	}

	if (DeployButton)
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] DeployButton is valid, binding click."), *GetName());
		DeployButton->OnClicked.AddDynamic(this, &UWhiskerwoodModListRowWidget::HandleDeployClicked);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] DeployButton is NULL in BindButtonsIfNeeded."), *GetName());
	}

	if (OpenFolderButton)
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] OpenFolderButton is valid, binding click."), *GetName());
		OpenFolderButton->OnClicked.AddDynamic(this, &UWhiskerwoodModListRowWidget::HandleOpenFolderClicked);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] OpenFolderButton is NULL in BindButtonsIfNeeded."), *GetName());
	}

	bButtonsBound = true;
}

void UWhiskerwoodModListRowWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	// If you want to be extra-safe, you can call Super here:
	// Super::NativeOnListItemObjectSet(ListItemObject);

	UE_LOG(LogTemp, Log, TEXT("[%s] NativeOnListItemObjectSet: %s"),
		*GetName(),
		*GetNameSafe(ListItemObject));

	ModItem = Cast<UWhiskerwoodModListItem>(ListItemObject);

	// Make sure the buttons are wired *now* that we have a ModItem
	BindButtonsIfNeeded();

	RefreshRow();
}

void UWhiskerwoodModListRowWidget::RefreshRow()
{
	UWhiskerwoodModToolsUILibrary::RefreshModListRowUI(
		ModItem,
		NameText,
		ChunkIdText,
		StatusText,
		MoveButton,
		RemoveButton,
		DeployButton
	);
}

void UWhiskerwoodModListRowWidget::HandleMoveClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[%s] HandleMoveClicked fired."), *GetName());

	if (!ModItem || !ModItem->ModSettings)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] HandleMoveClicked: ModItem or ModSettings is null."), *GetName());
		return;
	}

	UWhiskerwoodModToolsLibrary::MovePakMod(ModItem->ModSettings);
	RefreshRow();
}

void UWhiskerwoodModListRowWidget::HandleRemoveClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[%s] HandleRemoveClicked fired."), *GetName());

	if (!ModItem || !ModItem->ModSettings)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] HandleRemoveClicked: ModItem or ModSettings is null."), *GetName());
		return;
	}

	UWhiskerwoodModToolsLibrary::RemoveMovedMod(ModItem->ModSettings);
	RefreshRow();
}

void UWhiskerwoodModListRowWidget::HandleDeployClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[%s] HandleDeployClicked fired."), *GetName());

	if (!ModItem || !ModItem->ModSettings)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] HandleDeployClicked: ModItem or ModSettings is null."), *GetName());
		return;
	}

	FString StagingDir;
	UWhiskerwoodModToolsLibrary::DeployModToWorkshopStaging(
		ModItem->ModSettings,
		/*OutStagingDir*/ StagingDir
	);

	RefreshRow();
}

void UWhiskerwoodModListRowWidget::HandleOpenFolderClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[%s] HandleOpenFolderClicked fired."), *GetName());

	if (!ModItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] HandleOpenFolderClicked: ModItem is null."), *GetName());
		return;
	}

	UWhiskerwoodModToolsUILibrary::OpenModDirectoryForListItem(
		ModItem,
		/*bCreateIfMissing*/ true
	);
}