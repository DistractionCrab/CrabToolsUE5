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

/*Base object to be put into the tree view. Used to generate the widgest for the TreeView.*/
class FGraphDetailsViewItem: public TSharedFromThis<FGraphDetailsViewItem>
{
private:
	TArray<TSharedPtr<FGraphDetailsViewItem>> Children;

public:
	virtual ~FGraphDetailsViewItem() {}
	virtual TSharedRef<ITableRow> GetEntryWidget(
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly);

	void GetChildren(TArray<TSharedPtr<FGraphDetailsViewItem>>& OutChildren) const;
	void AddChild(TSharedPtr<FGraphDetailsViewItem> Item) { this->Children.Add(Item); }
};

class FHeaderItem : public FGraphDetailsViewItem
{
private:
	FText HeaderText;

public:
	virtual ~FHeaderItem() {}
	FHeaderItem(FText Text) : HeaderText(Text) {};
	FHeaderItem(FString String) : HeaderText(FText::FromString(String)) {};
	virtual TSharedRef<ITableRow> GetEntryWidget(
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly)
		override;
};

class FStateItem : public FGraphDetailsViewItem
{
private:
	UEdStateNode* NodeRef;

	TSharedPtr<SInlineEditableTextBlock> InlineText;

public:
	virtual ~FStateItem() {}
	FStateItem(UEdStateNode* NodePtr);

	virtual TSharedRef<ITableRow> GetEntryWidget(
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly)
		override;
private:
	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);
	bool IsSelected() const;
	void OnNameChanged(FName Name);
};

/**
 * The tab which shows an overview of the State Machine graph, such as
 * Edges, Nodes, Events, or Aliases.
 */
class STATEMACHINEEDITOR_API SGraphDetailsView 
: public SCompoundWidget, public FGCObject
{
private:
	TSharedPtr<class SSearchBox> FilterBox;
	TSharedPtr<STreeView<TSharedPtr<FGraphDetailsViewItem>>> StateListWidget;
	TArray<TSharedPtr<FGraphDetailsViewItem>> StateList;
	TSharedPtr<FGraphDetailsViewItem> StateRoot;

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
	void AddState(class UEdStateNode* Node);
	

	void OnItemSelected(TSharedPtr< FGraphDetailsViewItem > InSelectedItem, ESelectInfo::Type SelectInfo) {}
	void OnItemDoubleClicked(TSharedPtr< FGraphDetailsViewItem > InClickedItem) {}
	void OnGetChildrenForCategory(TSharedPtr<FGraphDetailsViewItem> InItem, TArray< TSharedPtr<FGraphDetailsViewItem> >& OutChildren);
	void OnItemScrolledIntoView(TSharedPtr<FGraphDetailsViewItem> InActionNode, const TSharedPtr<ITableRow>& InWidget) {}
	void OnSetExpansionRecursive(TSharedPtr<FGraphDetailsViewItem> InTreeNode, bool bInIsItemExpanded) {}

	// Section Functions.
	TSharedRef<ITableRow> OnGenerateRow(
		TSharedPtr<FGraphDetailsViewItem> InItem,
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly);
};