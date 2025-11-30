// WhiskerwoodModListItem.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WhiskerwoodModListItem.generated.h"

class UPDA_WhiskerwoodModSettings;

/**
 * Lightweight UObject used as a row item in the Editor Utility ListView
 * for mods. Wraps a UPDA_WhiskerwoodModSettings asset.
 */
UCLASS(BlueprintType) // <-- important!
class WHISKERWOOD_API UWhiskerwoodModListItem : public UObject
{
	GENERATED_BODY()

public:

	/** The backing Primary Data Asset that defines this mod. */
	UPROPERTY(BlueprintReadOnly, Category="Mod")
	UPDA_WhiskerwoodModSettings* ModSettings = nullptr;

	/** Convenience: Mod Name (copied from PDA). */
	UPROPERTY(BlueprintReadOnly, Category="Mod")
	FString ModName;

	/** Convenience: ChunkID (copied from PDA). */
	UPROPERTY(BlueprintReadOnly, Category="Mod")
	int32 ChunkID = 0;

	/** Convenience: Full Mods directory path for this mod. */
	UPROPERTY(BlueprintReadOnly, Category="Mod")
	FString ModDirectoryPath;
};
