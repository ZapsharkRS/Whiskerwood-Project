#include "WhiskerwoodEditorUserSettings.h"
#include "Misc/Paths.h"

UWhiskerwoodEditorUserSettings::UWhiskerwoodEditorUserSettings(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
#if WITH_EDITOR
    // Sensible defaults for editor usage
    if (ModsDirectory.IsEmpty())
    {
        ModsDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Mods"));
    }

    if (ProjectDir.IsEmpty())
    {
        ProjectDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
    }

    if (PlatformDir.IsEmpty())
    {
        PlatformDir = TEXT("Windows");
    }
#endif
}

void UWhiskerwoodEditorUserSettings::SaveSettings()
{
    SaveConfig();
}

UWhiskerwoodEditorUserSettings* UWhiskerwoodEditorUserSettings::GetSettings()
{
    return GetMutableDefault<UWhiskerwoodEditorUserSettings>();
}
