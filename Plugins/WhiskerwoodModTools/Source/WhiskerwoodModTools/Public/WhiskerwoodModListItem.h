// WhiskerwoodModListItem.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WhiskerwoodModListItem.generated.h"

class UPAL_WhiskerwoodModSettings;

/**
 * Lightweight UObject used as a row item in the Editor Utility ListView
 * for mods. Wraps a UPAL_WhiskerwoodModSettings asset.
 */
UCLASS(BlueprintType) // <-- important!
class WHISKERWOODMODTOOLS_API UWhiskerwoodModListItem : public UObject
{
	GENERATED_BODY()

public:

	/** The backing Primary Asset Label that defines this mod. */
	UPROPERTY(BlueprintReadOnly, Category="Whiskerwood|ModUtils|Widget Row")
	UPAL_WhiskerwoodModSettings* ModSettings = nullptr;

	/** Convenience: Mod Name (copied from PAL). */
	UPROPERTY(BlueprintReadOnly, Category="Whiskerwood|ModUtils|Widget Row")
	FString ModName;

	/** Convenience: ChunkID (copied from PAL). */
	UPROPERTY(BlueprintReadOnly, Category="Whiskerwood|ModUtils|Widget Row")
	int32 ChunkId = 0;

	/** Convenience: Full Mods directory path for this mod (Saved/mods/<ModDirName>). */
	UPROPERTY(BlueprintReadOnly, Category="Whiskerwood|ModUtils|Widget Row")
	FString ModDirectoryPath;
};
