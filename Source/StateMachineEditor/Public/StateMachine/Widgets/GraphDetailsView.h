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

class UEdEventObject;
class UEdTransition;
class FGraphDetailsViewItem;
using TablePtr = TSharedPtr<STreeView<TSharedPtr<FGraphDetailsViewItem>>>;
using TableWeakPtr = TWeakPtr<STreeView<TSharedPtr<FGraphDetailsViewItem>>>;

/*Base object to be put into the tree view. Used to generate the widgest for the TreeView.*/
class FGraphDetailsViewItem: public TSharedFromThis<FGraphDetailsViewItem>
{
private:
	TWeakPtr<FGraphDetailsViewItem> Parent;
	TableWeakPtr TableOwner;
	TArray<TSharedPtr<FGraphDetailsViewItem>> Children;

public:
	virtual ~FGraphDetailsViewItem() {}
	virtual TSharedRef<ITableRow> GetEntryWidget(
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly);

	void GetChildren(TArray<TSharedPtr<FGraphDetailsViewItem>>& OutChildren) const;
	void AddChild(TSharedPtr<FGraphDetailsViewItem> Item);
	void RemoveChild(TSharedPtr<FGraphDetailsViewItem> Item);
	void SetTableView(TableWeakPtr TableOwner);
	TWeakPtr<FGraphDetailsViewItem> GetParent() const { return this->Parent; }

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
	FText GetHeaderText() const { return this->HeaderText; }
};

/* Used for Events and Statics to add an Add button to the header. */
class FCreateHeaderItem : public FHeaderItem
{
protected:
	TWeakObjectPtr<UEdStateGraph> GraphReference;

public:
	virtual ~FCreateHeaderItem() {}
	FCreateHeaderItem(FText Text, TSharedPtr<class FEditor> InEditor);
	FCreateHeaderItem(FString String, TSharedPtr<class FEditor> InEditor)
		: FCreateHeaderItem(FText::FromString(String), InEditor) {}

	virtual TSharedRef<ITableRow> GetEntryWidget(
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly)
		override;

	FReply OnAddClicked() { return this->OnAddImplementation();  }
	virtual FReply OnAddImplementation() {  return FReply::Handled(); }
};

class FEventHeaderItem : public FCreateHeaderItem
{
	
public:
	FEventHeaderItem(FText Text, TSharedPtr<class FEditor> InEditor)
		: FCreateHeaderItem(Text, InEditor)	{}
	FEventHeaderItem(FString Text, TSharedPtr<class FEditor> InEditor)
		: FCreateHeaderItem(Text, InEditor)	{}

	virtual FReply OnAddImplementation() override;
};

class FStateItem : public FGraphDetailsViewItem
{
private:
	TWeakObjectPtr<UEdStateNode> NodeRef;

	TSharedPtr<SInlineEditableTextBlock> InlineText;

public:
	virtual ~FStateItem();
	FStateItem(UEdStateNode* NodePtr);

	virtual TSharedRef<ITableRow> GetEntryWidget(
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly)
		override;
private:
	
	bool OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage);
	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);
	bool IsSelected() const;
	void OnNameChanged(FName Name);
	void OnNodeDeleted();
};

class FEventItem : public FGraphDetailsViewItem
{
private:
	TSharedPtr<SInlineEditableTextBlock> InlineText;

protected:
	TWeakObjectPtr<UEdEventObject> EventReference;

public:
	FEventItem(TWeakObjectPtr<UEdEventObject> EventObject);

	virtual TSharedRef<ITableRow> GetEntryWidget(
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly)
		override;

private:
	bool IsSelected() const;
	bool OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage);
	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);

	void OnEventRenamed(FName To);
};

class FTransitionItem : public FGraphDetailsViewItem
{
private:
	TWeakObjectPtr<UEdTransition> NodeRef;

public:
	virtual ~FTransitionItem();
	FTransitionItem(UEdTransition* NodePtr);

	virtual TSharedRef<ITableRow> GetEntryWidget(
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly)
		override;

private:
	void OnNodeDeleted();
};

/**
 * The tab which shows an overview of the State Machine graph, such as
 * Edges, Nodes, Events, or Aliases.
 */
class STATEMACHINEEDITOR_API SGraphDetailsView 
: public SCompoundWidget, public FGCObject
{
private:
	TWeakPtr<FEditor> EditorPtr;
	TSharedPtr<class SSearchBox> FilterBox;
	TablePtr TreeView;
	TArray<TSharedPtr<FGraphDetailsViewItem>> TreeViewList;
	TSharedPtr<FGraphDetailsViewItem> StateRoot;
	TSharedPtr<FGraphDetailsViewItem> EventRoot;
	TSharedPtr<FGraphDetailsViewItem> AliasRoot;
	TSharedPtr<FGraphDetailsViewItem> StaticsRoot;

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
	void AddTransition(class UEdTransition* Node);

	void AddEvent(UEdEventObject* EventObject);

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

	void InitView(TSharedPtr<FEditor> InEditor);
};