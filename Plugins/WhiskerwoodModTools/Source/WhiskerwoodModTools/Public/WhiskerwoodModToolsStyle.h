#pragma once

#include "CoreMinimal.h"
#include "Styling/ISlateStyle.h"

class FWhiskerwoodModToolsStyle
{
public:
	static void Initialize();
	static void Shutdown();

	/** Get the style set name. */
	static FName GetStyleSetName();

	/** Shortcut to the style set itself. */
	static const ISlateStyle& Get();

private:
	static TSharedPtr<class FSlateStyleSet> StyleInstance;

	static TSharedRef<class FSlateStyleSet> Create();
};
