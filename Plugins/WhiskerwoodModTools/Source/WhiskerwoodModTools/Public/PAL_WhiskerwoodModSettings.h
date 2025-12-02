#pragma once

#include "CoreMinimal.h"
#include "Engine/PrimaryAssetLabel.h"
#include "PAL_WhiskerwoodModSettings.generated.h"

/**
 * Editor-only mod descriptor + chunk cooking label.
 *
 * This asset never ships with the game because all of its fields are
 * in WITH_EDITORONLY_DATA, which strips it during packaging.
 *
 * It drives mod metadata AND Primary Asset Label cooking rules.
 */
UCLASS(BlueprintType, EditInlineNew, meta=(DisplayName="Whiskerwood Mod Settings (PAL)"))
class WHISKERWOODMODTOOLS_API UPAL_WhiskerwoodModSettings : public UPrimaryAssetLabel
{
	GENERATED_BODY()

public:
	UPAL_WhiskerwoodModSettings();

	/** Force this PAL to use PrimaryAssetType "WhiskerwoodModSettings"
	 *  so it matches the AssetManagerSettings entry.
	 */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

#if WITH_EDITOR
	// Handle migration from old fields and keep Rules consistent.
	virtual void PostLoad() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

#if WITH_EDITORONLY_DATA

	/** Readable name for UI / workshop */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Mod Info")
	FString ModName;

	/** One-line description */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Mod Info")
	FString Description;

	/** Internal folder name for the mod (e.g. "MoreStarterWhiskers") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Mod Info")
	FString ModDirName;

	/** Version shown in the generated .uplugin (e.g. "1.0.0") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Mod Info")
	FString Version;

	/** Author name (e.g. "Shenjima") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Mod Info")
	FString CreatedBy;

	/**
	 * Optional Steam Workshop "publishedfileid" for this mod.
	 * Store the ID here after first publish / creation.
	 * Empty string means "no Workshop item yet".
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Whiskerwood|ModUtils|Mod Packaging|Steam")
	FString SteamWorkshopId;

	/**
	 * If true, every asset referenced by this PAL will be cooked.
	 * (This is the usual PrimaryAssetLabel "Cook Rule" default.)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rules")
	bool bCookAll = true;

	// ----------------------------------------------------------------------
	// Chunk ID configuration
	// ----------------------------------------------------------------------

	// Legacy field so existing PAL assets don’t explode.
	// This used to be your “ChunkID” in the Whiskerwood|ModUtils|Cooking section.
	UPROPERTY(EditAnywhere, Category="Whiskerwood|ModUtils|Deprecated", meta=(
		DeprecatedProperty,
		DeprecationMessage="Use the Chunk ID in the Rules category instead."
	))
	int32 LegacyChunkID = -1;

	/**
	 * If true, your helper logic can derive ChunkId from the mod folder
	 * name (e.g. hash or mapping). If false, OverrideChunkId is used.
	 *
	 * NOTE: The actual engine chunk used by UAT is Rules.ChunkId;
	 * this flag just controls how GetChunkId() decides what to return.
	 */
	UPROPERTY(EditAnywhere, Category="Whiskerwood|ModUtils|Cooking")
	bool bUseModFolderNameForChunk = true;

	/**
	 * Explicit chunk to use when bUseModFolderNameForChunk == false.
	 * GetChunkId() will prefer this when > 0.
	 */
	UPROPERTY(EditAnywhere, Category="Whiskerwood|ModUtils|Cooking", meta=(EditCondition="!bUseModFolderNameForChunk"))
	int32 OverrideChunkId = -1;

	/**
	 * Returns the effective ChunkId this mod should use for its .pak.
	 * This is what Move/Remove/Deploy should call.
	 *
	 * Priority:
	 *   1) If !bUseModFolderNameForChunk and OverrideChunkId > 0 → OverrideChunkId
	 *   2) If Rules.ChunkId > 0 → Rules.ChunkId (per-asset override)
	 *   3) If LegacyChunkID > 0 → LegacyChunkID (migration)
	 *   4) Otherwise → AssetManager effective rules (type-level, i.e. 20)
	 */
	UFUNCTION(BlueprintPure, Category="Whiskerwood|ModUtils|Cooking")
	int32 GetChunkId() const;

#endif // WITH_EDITORONLY_DATA
};
