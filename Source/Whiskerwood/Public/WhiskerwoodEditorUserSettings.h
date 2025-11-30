#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WhiskerwoodEditorUserSettings.generated.h"

/**
 * Per-user editor settings for Whiskerwood tools.
 * Stored in EditorPerProjectUserSettings.ini.
 */
UCLASS(Config=EditorPerProjectUserSettings, BlueprintType)
class WHISKERWOOD_API UWhiskerwoodEditorUserSettings : public UObject
{
	GENERATED_BODY()

public:

	// Use FObjectInitializer-style ctor so it matches what UHT expects
	UWhiskerwoodEditorUserSettings(const FObjectInitializer& ObjectInitializer);

	/** Directory where mods (.pak files) are deployed / stored. */
	UPROPERTY(EditAnywhere, Config, Category="Whiskerwood|Modding")
	FString ModsDirectory;

	/** Last copied .pak file path. */
	UPROPERTY(EditAnywhere, Config, Category="Whiskerwood|Modding")
	FString LastCopiedPakPath;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Whiskerwood|Modding")
	FString ProjectDir;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Whiskerwood|Modding")
	FString AppDataDir;

	UPROPERTY(EditAnywhere, Config, BlueprintReadWrite, Category="Whiskerwood|Modding")
	FString PlatformDir;

public:

	/** Writes UPROPERTY(Config) values to EditorPerProjectUserSettings.ini */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|Modding", meta=(DevelopmentOnly))
	void SaveSettings();

	/** Gets the singleton settings object instance */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|Modding", meta=(DevelopmentOnly))
	static UWhiskerwoodEditorUserSettings* GetSettings();
};
