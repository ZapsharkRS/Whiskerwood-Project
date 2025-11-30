#include "PDA_WhiskerwoodModSettings.h"

FPrimaryAssetId UPDA_WhiskerwoodModSettings::GetPrimaryAssetId() const
{
	// Give all these assets a type of "WhiskerwoodMod" so they can be grouped in the Primary Asset system if desired.
	static const FPrimaryAssetType ModType = TEXT("WhiskerwoodMod");

	return FPrimaryAssetId(ModType, GetFName());
}
