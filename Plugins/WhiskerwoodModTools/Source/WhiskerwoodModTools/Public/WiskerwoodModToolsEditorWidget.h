#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"

#include "WiskerwoodModToolsEditorWidget.generated.h"

class UWhiskerwoodLogWatcherSubsystem;

/**
 * Base Editor Utility Widget for Whiskerwood modding tools.
 */
UCLASS(Abstract)
class WHISKERWOODMODTOOLS_API UWiskerwoodModToolsEditorWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|Subsystems|LogWatcher")
	UWhiskerwoodLogWatcherSubsystem* GetLogWatcher() const;

protected:

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category="Whiskerwood|ModUtils|Subsystems|LogWatcher")
	void HandleLogLines(
		FName ConfigId,
		const FString& DisplayName,
		const FString& FilePath,
		const TArray<FString>& NewLines
	);

private:

	UFUNCTION()
	void OnLogLines_Internal(
		FName ConfigId,
		const FString& DisplayName,
		const FString& FilePath,
		const TArray<FString>& NewLines
	);
};
