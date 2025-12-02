#include "WhiskerwoodModToolsStyle.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Brushes/SlateImageBrush.h"

TSharedPtr<FSlateStyleSet> FWhiskerwoodModToolsStyle::StyleInstance = nullptr;

// Helper macro for image brush
#define IMAGE_BRUSH(RelativePath, Size) FSlateImageBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), Size)

void FWhiskerwoodModToolsStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FWhiskerwoodModToolsStyle::Shutdown()
{
	if (StyleInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
		StyleInstance.Reset();
	}
}

FName FWhiskerwoodModToolsStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("WhiskerwoodModToolsStyle"));
	return StyleSetName;
}

const ISlateStyle& FWhiskerwoodModToolsStyle::Get()
{
	return *StyleInstance.Get();
}

TSharedRef<FSlateStyleSet> FWhiskerwoodModToolsStyle::Create()
{
	TSharedRef<FSlateStyleSet> StyleSet = MakeShareable(
		new FSlateStyleSet(FWhiskerwoodModToolsStyle::GetStyleSetName())
	);

	// Point to the plugin's Resources folder
	const FString ContentDir = IPluginManager::Get()
		.FindPlugin(TEXT("WhiskerwoodModTools"))
		->GetBaseDir() / TEXT("Resources");

	StyleSet->SetContentRoot(ContentDir);

	// Register our toolbar icon
	const FVector2D IconSize(40.0f, 40.0f);
	StyleSet->Set(
		TEXT("WhiskerwoodModTools.ToolbarIcon"),
		new IMAGE_BRUSH(TEXT("Icon_WWModTools_40"), IconSize)
	);

	return StyleSet;
}

#undef IMAGE_BRUSH
