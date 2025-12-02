#include "WhiskerwoodPageSwitcherWidget.h"

#include "Components/WidgetSwitcher.h"

UWhiskerwoodPageSwitcherWidget::UWhiskerwoodPageSwitcherWidget(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ActivePageId(NAME_None)
{
}

void UWhiskerwoodPageSwitcherWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// If user didn't configure Pages explicitly, auto-build from children
	BuildPagesFromSwitcherIfEmpty();

	// Optionally enforce that PageSwitcher has enough children for the indices
	if (PageSwitcher)
	{
		const int32 NumChildren = PageSwitcher->GetNumWidgets();
		for (const FWhiskerwoodPageDef& Page : Pages)
		{
			if (Page.SwitcherIndex < 0 || Page.SwitcherIndex >= NumChildren)
			{
				UE_LOG(LogTemp, Warning,
					TEXT("WhiskerwoodPageSwitcherWidget: Page '%s' has invalid SwitcherIndex %d (NumWidgets=%d)"),
					*Page.PageId.ToString(), Page.SwitcherIndex, NumChildren);
			}
		}
	}

	// Optionally select first valid page on init
	for (const FWhiskerwoodPageDef& Page : Pages)
	{
		if (!Page.PageId.IsNone())
		{
			ShowPageById(Page.PageId);
			break;
		}
	}
}

void UWhiskerwoodPageSwitcherWidget::BuildPagesFromSwitcherIfEmpty()
{
	if (!PageSwitcher)
	{
		return;
	}

	if (Pages.Num() > 0)
	{
		// User configured them manually; don't override.
		return;
	}

	const int32 NumChildren = PageSwitcher->GetNumWidgets();
	for (int32 Index = 0; Index < NumChildren; ++Index)
	{
		if (UWidget* Child = PageSwitcher->GetWidgetAtIndex(Index))
		{
			const FName ChildName = Child->GetFName();

			FWhiskerwoodPageDef NewPage;
			NewPage.PageId = ChildName;              // Use widget name as Id
			NewPage.DisplayName = FText::FromName(ChildName);
			NewPage.SwitcherIndex = Index;

			Pages.Add(NewPage);
		}
	}

	if (Pages.Num() == 0)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("WhiskerwoodPageSwitcherWidget: PageSwitcher has no children; no pages to build."));
	}
	else
	{
		UE_LOG(LogTemp, Log,
			TEXT("WhiskerwoodPageSwitcherWidget: Auto-built %d pages from PageSwitcher children."), Pages.Num());
	}
}

bool UWhiskerwoodPageSwitcherWidget::ShowPageById(FName PageId)
{
	if (!PageSwitcher || PageId.IsNone())
	{
		return false;
	}

	for (const FWhiskerwoodPageDef& Page : Pages)
	{
		if (Page.PageId == PageId)
		{
			return ShowPageByIndex(Page.SwitcherIndex);
		}
	}

	UE_LOG(LogTemp, Warning,
		TEXT("WhiskerwoodPageSwitcherWidget: No page with Id '%s' found."),
		*PageId.ToString());
	return false;
}

bool UWhiskerwoodPageSwitcherWidget::ShowPageByIndex(int32 Index)
{
	if (!PageSwitcher)
	{
		return false;
	}

	const int32 NumChildren = PageSwitcher->GetNumWidgets();
	if (Index < 0 || Index >= NumChildren)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("WhiskerwoodPageSwitcherWidget: Invalid index %d (NumWidgets=%d)."),
			Index, NumChildren);
		return false;
	}

	PageSwitcher->SetActiveWidgetIndex(Index);

	// Update ActivePageId from the Pages array if possible
	FName NewId = NAME_None;
	for (const FWhiskerwoodPageDef& Page : Pages)
	{
		if (Page.SwitcherIndex == Index)
		{
			NewId = Page.PageId;
			break;
		}
	}

	ActivePageId = NewId;

	OnActivePageChanged(ActivePageId, Index);

	return true;
}

FText UWhiskerwoodPageSwitcherWidget::GetDisplayNameForPageId(FName PageId) const
{
	for (const FWhiskerwoodPageDef& Page : Pages)
	{
		if (Page.PageId == PageId)
		{
			return Page.DisplayName;
		}
	}

	return FText::GetEmpty();
}
