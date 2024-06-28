#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Misc/NotifyHook.h"

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SExpanderArrow.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/STreeView.h"
#include "Widgets/Layout/SWidgetSwitcher.h"

#include "StateMachine/Editor.h"

class UStateMachineBlueprint;
class UEdStateGraph;
class UEdEventObject;
class UEdTransition;
class FGraphDetailsViewItem;
using TablePtr = TSharedPtr<STreeView<TSharedPtr<FGraphDetailsViewItem>>>;
using TableWeakPtr = TWeakPtr<STreeView<TSharedPtr<FGraphDetailsViewItem>>>;

/*Base object to be put into the tree view. Used to generate the widgest for the TreeView.*/
class FGraphDetailsViewItem : public TSharedFromThis<FGraphDetailsViewItem>
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
	void AddChild(TSharedPtr<FGraphDetailsViewItem> Item, bool DeferRefresh = false);
	void RemoveChild(TSharedPtr<FGraphDetailsViewItem> Item);
	void SetTableView(TableWeakPtr TableOwner);
	TWeakPtr<FGraphDetailsViewItem> GetParent() const { return this->Parent; }
	virtual void Select() {}
	virtual void InitView() {}
};

class FHeaderItem : public FGraphDetailsViewItem
{
private:

	FText HeaderText;

public:
	virtual ~FHeaderItem() {}
	FHeaderItem(FText Text)
		: HeaderText(Text) {}
	FHeaderItem(FString String)
		: HeaderText(FText::FromString(String)) {}

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

	TWeakObjectPtr<UEdStateGraph> GraphRef;

public:

	virtual ~FCreateHeaderItem() {}
	FCreateHeaderItem(UEdStateGraph* Graph, FText Text);
	FCreateHeaderItem(UEdStateGraph* Graph, FString String)
		: FCreateHeaderItem(Graph, FText::FromString(String)) {}

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
	
	FEventHeaderItem(UEdStateGraph* Graph, FText Text)
		: FCreateHeaderItem(Graph, Text) {}
	FEventHeaderItem(UEdStateGraph* Graph, FString Text)
		: FCreateHeaderItem(Graph, Text) {}
	

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
	void OnNameChanged(FName Name);
	void OnNodeDeleted();

	void Select() override { this->NodeRef->Inspect(); }
};

class FStateMachineItem : public FGraphDetailsViewItem
{
private:
	TWeakObjectPtr<UEdStateGraph> GraphRef;

	TSharedPtr<FGraphDetailsViewItem> StateRoot;
	TSharedPtr<FGraphDetailsViewItem> EventRoot;
	TSharedPtr<FGraphDetailsViewItem> AliasRoot;
	TSharedPtr<FGraphDetailsViewItem> StaticsRoot;

	TSharedPtr<SInlineEditableTextBlock> InlineText;

public:
	virtual ~FStateMachineItem();
	FStateMachineItem(UEdStateGraph* GraphPtr);

	virtual TSharedRef<ITableRow> GetEntryWidget(
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly)
		override;

	void AddState(UEdStateNode* State, bool DeferRefresh);

private:

	bool OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage);
	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);
	void OnNameChanged(FName Name);
	void OnNodeDeleted();
	void OnEventCreated(UEdEventObject* Event, bool DeferRefresh);
	void OnGraphChanged(const FEdGraphEditAction& Action);

	void Select() override 
	{
		UE_LOG(LogTemp, Warning, TEXT("Selecting a Graph."));
		this->GraphRef.Get()->Inspect();
		this->GraphRef->Select();
	}
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
	bool OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage);
	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);

	void OnEventRenamed(FName To);

	void Select() { this->EventReference->Inspect(); }
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

/* Widget responsible for listing the details of individual state machines. */
class STATEMACHINEEDITOR_API SSubGraphDetails
: public SCompoundWidget, public FGCObject
{
private:
	TWeakObjectPtr<UEdStateGraph> GraphRef;
	TablePtr TreeView;

	TSharedPtr<FGraphDetailsViewItem> StateRoot;
	TSharedPtr<FGraphDetailsViewItem> EventRoot;
	TSharedPtr<FGraphDetailsViewItem> AliasRoot;
	TSharedPtr<FGraphDetailsViewItem> StaticsRoot;

	// TreeView array.
	TArray<TSharedPtr<FGraphDetailsViewItem>> TreeViewList;

	SLATE_BEGIN_ARGS(SSubGraphDetails) {}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, UEdStateGraph* GraphPtr);

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override
	{
		return TEXT("SSubGraphDetails");
	}

	void AddState(UEdStateNode* Node, bool DeferRefresh);
	void AddEvent(UEdEventObject* Node, bool DeferRefresh);
	void Refresh() { this->TreeView->RequestListRefresh(); }

private:
	void OnItemSelected(TSharedPtr< FGraphDetailsViewItem > InSelectedItem, ESelectInfo::Type SelectInfo) {}
	void OnItemDoubleClicked(TSharedPtr< FGraphDetailsViewItem > InClickedItem) {}
	void OnGetChildrenForCategory(TSharedPtr<FGraphDetailsViewItem> InItem, TArray< TSharedPtr<FGraphDetailsViewItem> >& OutChildren);
	void OnItemScrolledIntoView(TSharedPtr<FGraphDetailsViewItem> InActionNode, const TSharedPtr<ITableRow>& InWidget) {}
	void OnSetExpansionRecursive(TSharedPtr<FGraphDetailsViewItem> InTreeNode, bool bInIsItemExpanded) {}
	TSharedRef<ITableRow> OnGenerateRow(
		TSharedPtr<FGraphDetailsViewItem> InItem,
		const TSharedRef<STableViewBase>& OwnerTable,
		bool bIsReadOnly);

	void OnSelectionChanged(
		TSharedPtr<FGraphDetailsViewItem> SelectedItem,
		ESelectInfo::Type SelectInfo);

	void BindEvents();

	void OnStateAdded(UEdStateNode* State);
	void OnEventAdded(UEdEventObject* Event);
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
	TSharedPtr<SWidgetSwitcher> DetailsTabs;
	TMap<UEdStateGraph*, TSharedPtr<SSubGraphDetails>> GraphToWidgetMap;

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
	FReply OnAddStateMachine();
	void BindEvents(TSharedPtr<class FEditor> InEditor);
	void InitView(TSharedPtr<FEditor> InEditor);	
	void InitGraphDetailsView(UEdStateGraph* Graph);

	// Event Handlers for State Machine changes.
	void OnGraphSelected(UEdStateGraph* Graph);
	void OnStateAdded(UEdStateNode* Node);

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

	

	void OnSelectionChanged(
		TSharedPtr<FGraphDetailsViewItem> SelectedItem,
		ESelectInfo::Type SelectInfo);
};