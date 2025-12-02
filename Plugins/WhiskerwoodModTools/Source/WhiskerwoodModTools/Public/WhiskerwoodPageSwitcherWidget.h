#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WhiskerwoodPageSwitcherWidget.generated.h"

class UWidgetSwitcher;

/**
 * Simple data for one logical page in the switcher.
 */
USTRUCT(BlueprintType)
struct WHISKERWOODMODTOOLS_API FWhiskerwoodPageDef
{
	GENERATED_BODY()

public:

	/** Stable logical ID you use from code/Blueprint, e.g. "Dashboard", "Logs" */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Whiskerwood|ModUtils|PageSwitcher|Page")
	FName PageId;

	/** Optional display text for UI / tabs */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Whiskerwood|ModUtils|PageSwitcher|Page")
	FText DisplayName;

	/** Index in the underlying UWidgetSwitcher */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Whiskerwood|ModUtils|PageSwitcher|Page")
	int32 SwitcherIndex = 0;

	FWhiskerwoodPageDef()
		: PageId(NAME_None)
		, SwitcherIndex(0)
	{
	}
};

/**
 * Lightweight wrapper around a UWidgetSwitcher that lets you work with
 * named pages instead of raw indices.
 *
 * Usage:
 *  - Make a BP widget inheriting from this C++ class.
 *  - Add a WidgetSwitcher to the designer and name it "PageSwitcher".
 *  - EITHER:
 *      - Fill Pages[] manually, OR
 *      - Leave Pages empty and it will auto-map children by widget name.
 *  - In your top menu / tab buttons, call ShowPageById("Logs"), etc.
 */
UCLASS()
class WHISKERWOODMODTOOLS_API UWhiskerwoodPageSwitcherWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UWhiskerwoodPageSwitcherWidget(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void NativeOnInitialized() override;

	/** The underlying widget switcher you place in the BP designer. */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> PageSwitcher;

	/** Declarative list of pages. Configure this in the BP defaults panel. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Whiskerwood|ModUtils|PageSwitcher")
	TArray<FWhiskerwoodPageDef> Pages;

	/** Id of the currently active page (if known). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Whiskerwood|ModUtils|PageSwitcher")
	FName ActivePageId;

public:

	/** Show a page by logical Id. Returns false if Id not found. */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|PageSwitcher")
	bool ShowPageById(FName PageId);

	/** Show a page by index; updates ActivePageId if there is a matching entry. */
	UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|PageSwitcher")
	bool ShowPageByIndex(int32 Index);

	/** Get Id of currently active page (NAME_None if unknown) */
	UFUNCTION(BlueprintPure, Category="Whiskerwood|ModUtils|PageSwitcher")
	FName GetActivePageId() const { return ActivePageId; }

	/** Find the configured display name for a PageId (or empty text). */
	UFUNCTION(BlueprintPure, Category="Whiskerwood|ModUtils|PageSwitcher")
	FText GetDisplayNameForPageId(FName PageId) const;

	/** Fired whenever the active page changes. */
	UFUNCTION(BlueprintImplementableEvent, Category="Whiskerwood|ModUtils|PageSwitcher")
	void OnActivePageChanged(FName NewPageId, int32 NewIndex);

private:

	/** If Pages is empty, auto-populate it from PageSwitcher children. */
	void BuildPagesFromSwitcherIfEmpty();
};
