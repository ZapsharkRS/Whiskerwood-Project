#include "WiskerwoodModToolsEditorWidget.h"

#include "WhiskerwoodLogWatcherSubsystem.h"

#if WITH_EDITOR
#include "Editor.h"
#endif

UWhiskerwoodLogWatcherSubsystem* UWiskerwoodModToolsEditorWidget::GetLogWatcher() const
{

	if (GEditor)
	{
		return GEditor->GetEditorSubsystem<UWhiskerwoodLogWatcherSubsystem>();
	}

	return nullptr;
}

void UWiskerwoodModToolsEditorWidget::NativeConstruct()
{
	Super::NativeConstruct();


	if (UWhiskerwoodLogWatcherSubsystem* Subsystem = GetLogWatcher())
	{
		Subsystem->OnLogLines.AddDynamic(
			this,
			&UWiskerwoodModToolsEditorWidget::OnLogLines_Internal
		);
	}

}

void UWiskerwoodModToolsEditorWidget::NativeDestruct()
{

	if (UWhiskerwoodLogWatcherSubsystem* Subsystem = GetLogWatcher())
	{
		Subsystem->OnLogLines.RemoveDynamic(
			this,
			&UWiskerwoodModToolsEditorWidget::OnLogLines_Internal
		);
	}


	Super::NativeDestruct();
}

void UWiskerwoodModToolsEditorWidget::OnLogLines_Internal(
	FName ConfigId,
	const FString& DisplayName,
	const FString& FilePath,
	const TArray<FString>& NewLines
)
{
	// Route into BP; BP may or may not implement this.
	HandleLogLines(ConfigId, DisplayName, FilePath, NewLines);
}
