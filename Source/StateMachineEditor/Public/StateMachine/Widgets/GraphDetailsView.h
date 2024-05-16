#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Misc/NotifyHook.h"

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SExpanderArrow.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/STreeView.h"

#include "StateMachine/Editor.h"

class IDetailsView;

/**
 * The tab which shows an overview of the State Machine graph, such as
 * Edges, Nodes, Events, or Aliases.
 */
class STATEMACHINEEDITOR_API SGraphDetailsView 
: public SCompoundWidget, public FGCObject
{
private:
	TSharedPtr<class SSearchBox> FilterBox;
	TSharedPtr<STreeView<TSharedPtr<class FStateListEntry>>> StateListWidget;
	TArray<TSharedPtr<class FStateListEntry>> StateList;
	//TSharedPtr<STreeView<TSharedPtr<class FEventListEntry>>> EventListWidget;
	//TSharedPtr<STreeView<TSharedPtr<class FAliasListEntry>>> AliasListWidget;

public:
	SLATE_BEGIN_ARGS(SGraphDetailsView){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedPtr<class FEditor> InEditor);

	virtual FString GetReferencerName() const override
	{
		return TEXT("SGraphDetailsView");
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	
private:
	void BindEvents(TSharedPtr<class FEditor> InEditor);
	void OnGraphChanged(const FEdGraphEditAction& Action);
	void AddState(const class UEdStateNode* Node);
	TSharedRef<ITableRow> OnGenerateRow(
		TSharedPtr<FStateListEntry> InItem, 
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly);

	void OnItemSelected(TSharedPtr< FStateListEntry > InSelectedItem, ESelectInfo::Type SelectInfo) {}
	void OnItemDoubleClicked(TSharedPtr< FStateListEntry > InClickedItem) {}
	void OnGetChildrenForCategory(TSharedPtr<FStateListEntry> InItem, TArray< TSharedPtr<FStateListEntry> >& OutChildren) {}
	void OnItemScrolledIntoView(TSharedPtr<FStateListEntry> InActionNode, const TSharedPtr<ITableRow>& InWidget) {}
	void OnSetExpansionRecursive(TSharedPtr<FStateListEntry> InTreeNode, bool bInIsItemExpanded) {}

	// Section Functions.
	FText OnGetSectionTitle(int32 InSectionID);
	TSharedRef<SWidget> OnGetSectionWidget(TSharedRef<SWidget> RowWidget, int32 InSectionID);
};