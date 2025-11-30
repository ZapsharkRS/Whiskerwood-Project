#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PDA_WhiskerwoodModSettings.generated.h"

/**
 * Primary Data Asset describing a Whiskerwood mod.
 *
 * One asset per mod, named in PascalCase (e.g. PDA_MoreStarterWhiskers).
 * This drives:
 *  - Mod display in your dashboard
 *  - .uplugin (or JSON) descriptor generation
 *  - ChunkID / directory naming
 */
UCLASS(BlueprintType)
class WHISKERWOOD_API UPDA_WhiskerwoodModSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	// Optional: override to give this asset a custom primary asset type.
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	/** Human-readable mod name (will also be used in descriptor Name) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mod Info")
	FString ModName;

	/** Description shown in the dashboard and in the .uplugin descriptor */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mod Info", meta=(MultiLine=true))
	FString Description;

	/** Version string, e.g. "1.0.0" */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mod Info")
	FString Version;

	/** Author / creator name, e.g. "Shenjima" */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mod Info")
	FString CreatedBy;

	/**
	 * Directory name used for this mod inside the mods folder.
	 * For example: "MoreStarterWhiskers"
	 *
	 * Final path will typically be:
	 *   <AppData>/Local/Whiskerwood/Saved/mods/<ModDirName>/
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mod Packaging")
	FString ModDirName;

	/**
	 * Chunk ID to use for this mod's .pak, as established in PAL.
	 * e.g., 1001, 1002, etc.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Mod Packaging")
	int32 ChunkID = 0;
};
