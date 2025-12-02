// Private/PAL_WhiskerwoodModSettings.cpp

#include "PAL_WhiskerwoodModSettings.h"
#include "Misc/Paths.h"
#include "Engine/AssetManager.h"  // For FPrimaryAssetId / FPrimaryAssetType / GetPrimaryAssetRules

UPAL_WhiskerwoodModSettings::UPAL_WhiskerwoodModSettings()
{
    // Allow manually marking assets
    bCookAll = true;

    // User will fill these, but here are safe defaults
    if (ModDirName.IsEmpty())
    {
        ModDirName = TEXT("NewMod");
    }

    // Primary Asset Label defaults
    bLabelAssetsInMyDirectory = true;
}

FPrimaryAssetId UPAL_WhiskerwoodModSettings::GetPrimaryAssetId() const
{
    // Force type to match AssetManagerSettings PrimaryAssetTypesToScan entry:
    // PrimaryAssetType="WhiskerwoodModSettings"
    static const FPrimaryAssetType ModType(TEXT("WhiskerwoodModSettings"));
    return FPrimaryAssetId(ModType, GetFName());
}

#if WITH_EDITOR

void UPAL_WhiskerwoodModSettings::PostLoad()
{
    Super::PostLoad();

#if WITH_EDITORONLY_DATA
    // Migration: if this PAL came from an old version that used LegacyChunkID,
    // and the real engine Rules.ChunkId is still at its default, copy the value over.
    if (LegacyChunkID > 0 && Rules.ChunkId <= 0)
    {
        Rules.ChunkId = LegacyChunkID;
    }
#endif
}

void UPAL_WhiskerwoodModSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

#if WITH_EDITORONLY_DATA
    if (FProperty* ChangedProp = PropertyChangedEvent.Property)
    {
        const FName PropName = ChangedProp->GetFName();

        // If someone still edits the old LegacyChunkID, keep Rules.ChunkId in sync.
        if (PropName == GET_MEMBER_NAME_CHECKED(UPAL_WhiskerwoodModSettings, LegacyChunkID))
        {
            if (LegacyChunkID > 0)
            {
                Rules.ChunkId = LegacyChunkID;
            }
        }

        // Optional: if you ever expose OverrideChunkId in the UI later,
        // you could force bUseModFolderNameForChunk=false here when it changes.
    }
#endif
}

#endif // WITH_EDITOR

#if WITH_EDITORONLY_DATA
int32 UPAL_WhiskerwoodModSettings::GetChunkId() const
{
    // 1) Explicit override wins when not deriving from folder name
    if (!bUseModFolderNameForChunk && OverrideChunkId > 0)
    {
        return OverrideChunkId;
    }

    // 2) If this asset has an explicit Rules.ChunkId set, use that
    if (Rules.ChunkId > 0)
    {
        return Rules.ChunkId;
    }

    // 3) If we still have a LegacyChunkID, prefer that
    if (LegacyChunkID > 0)
    {
        return LegacyChunkID;
    }

    // 4) Ask the Asset Manager for the *effective* rules, which
    // merges type-level + per-asset rules (this is where your
    // WhiskerwoodModSettings ChunkId=20 from INI actually lives).
    if (UAssetManager* Manager = UAssetManager::GetIfInitialized())
    {
        const FPrimaryAssetId Id = GetPrimaryAssetId();
        if (Id.IsValid())
        {
            const FPrimaryAssetRules EffectiveRules = Manager->GetPrimaryAssetRules(Id);
            if (EffectiveRules.ChunkId > 0)
            {
                return EffectiveRules.ChunkId;
            }
        }
    }


    // 5) No chunk configured
    return -1;
}
#endif
