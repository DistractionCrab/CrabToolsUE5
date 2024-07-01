#include "StateMachine/Widgets/GraphDetailsView.h"

#include "Widgets/SWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#include "StateMachine/EdGraph/EdTransition.h"
#include "StateMachine/EdGraph/EdEventObject.h"
#include "GraphEditAction.h"
#include "StateMachine/StateMachineBlueprint.h"

#define LOCTEXT_NAMESPACE "PSM"

template<typename ItemType>
class SCategoryHeaderTableRow : public STableRow<ItemType>
{
private:
	TSharedPtr<SBorder> ContentBorder;

public:
	SLATE_BEGIN_ARGS(SCategoryHeaderTableRow) {}
		SLATE_DEFAULT_SLOT(typename SCategoryHeaderTableRow::FArguments, Content)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
	{
		STableRow<ItemType>::ChildSlot
			.Padding(0.0f, 2.0f, .0f, 0.0f)
			[
				SAssignNew(ContentBorder, SBorder)
				.BorderImage(this, &SCategoryHeaderTableRow::GetBackgroundImage)
				.Padding(FMargin(3.0f, 5.0f))
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.Padding(5.0f)
					.AutoWidth()
					[
						SNew(SExpanderArrow, STableRow< ItemType >::SharedThis(this))
					]
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.AutoWidth()
					[
						InArgs._Content.Widget
					]
				]
			];

		STableRow < ItemType >::ConstructInternal(
			typename STableRow< ItemType >::FArguments()
			.Style(FAppStyle::Get(), "DetailsView.TreeView.TableRow")
			.ShowSelection(false),
			InOwnerTableView
		);
	}

	const FSlateBrush* GetBackgroundImage() const
	{
		if (STableRow<ItemType>::IsHovered())
		{
			return FAppStyle::Get().GetBrush("Brushes.Secondary");
		}
		else
		{
			return FAppStyle::Get().GetBrush("Brushes.Header");
		}
	}

	virtual void SetContent(TSharedRef< SWidget > InContent) override
	{
		ContentBorder->SetContent(InContent);
	}

	virtual void SetRowContent(TSharedRef< SWidget > InContent) override
	{
		ContentBorder->SetContent(InContent);
	}

	virtual const FSlateBrush* GetBorder() const
	{
		return nullptr;
	}

	FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
	{
		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			STableRow<ItemType>::ToggleExpansion();
			return FReply::Handled();
		}
		else
		{
			return FReply::Unhandled();
		}
	}
};

#pragma region FGraphDetailsViewItem

void FGraphDetailsViewItem::AddChild(TSharedPtr<FGraphDetailsViewItem> Item, bool DeferRefresh)
{ 
	Item->Parent = this->AsWeak();
	this->Children.Add(Item);
	Item->SetTableView(this->TableOwner);

	if (!DeferRefresh && this->TableOwner.IsValid())
	{
		this->TableOwner.Pin()->RequestListRefresh();
	}
}

void FGraphDetailsViewItem::RemoveChild(TSharedPtr<FGraphDetailsViewItem> Item)
{
	this->Children.Remove(Item);
	this->TableOwner.Pin()->RequestListRefresh();
}

void FGraphDetailsViewItem::SetTableView(TableWeakPtr NewTableOwner)
{
	this->TableOwner = NewTableOwner;
}

TSharedRef<ITableRow> FGraphDetailsViewItem::GetEntryWidget(
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	// This widget is only used for debug purposes.
	TSharedPtr<IToolTip> SectionToolTip;
	TSharedPtr<STableRow<TSharedPtr<FGraphDetailsViewItem>>> TableRow;

	TableRow = SNew(SCategoryHeaderTableRow<TSharedPtr<FGraphDetailsViewItem>>, OwnerTable)
		.ToolTip(SectionToolTip);
	TableRow->SetRowContent(SNew(STextBlock).Text(FText::FromString("NULL WIDGET")));

	return TableRow.ToSharedRef();
}

#pragma endregion

TSharedRef<ITableRow> FHeaderItem::GetEntryWidget(
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	TSharedPtr<IToolTip> SectionToolTip;
	TSharedPtr<STableRow<TSharedPtr<FGraphDetailsViewItem>>> TableRow;

	TableRow = SNew(SCategoryHeaderTableRow<TSharedPtr<FGraphDetailsViewItem>>, OwnerTable)
		.ToolTip(SectionToolTip);

	TSharedPtr<SHorizontalBox> RowContainer;
	TableRow->SetRowContent
	(
		SAssignNew(RowContainer, SHorizontalBox)
	);

	TSharedPtr<SWidget> RowContent = SNew(SRichTextBlock)
		.Text(this->HeaderText)
		.TransformPolicy(ETextTransformPolicy::ToUpper)
		.DecoratorStyleSet(&FAppStyle::Get())
		.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle");

	FMargin RowPadding = FMargin(0, 2);

	TSharedPtr<SExpanderArrow> ExpanderWidget;

	ExpanderWidget =
		SNew(SExpanderArrow, TableRow)
		.BaseIndentLevel(1);

	RowContainer->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Right)
		[
			ExpanderWidget.ToSharedRef()
		];

	RowContainer->AddSlot()
		.FillWidth(1.0)
		.Padding(RowPadding)
		[
			RowContent.ToSharedRef()
		];

	return TableRow.ToSharedRef();
}

FCreateHeaderItem::FCreateHeaderItem(UEdStateGraph* Graph, FText Text)
: FHeaderItem(Text),
	GraphRef(Graph)
	
{

}

TSharedRef<ITableRow> FCreateHeaderItem::GetEntryWidget(
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	TSharedPtr<IToolTip> SectionToolTip;
	TSharedPtr<STableRow<TSharedPtr<FGraphDetailsViewItem>>> TableRow;

	TableRow = SNew(SCategoryHeaderTableRow<TSharedPtr<FGraphDetailsViewItem>>, OwnerTable)
		.ToolTip(SectionToolTip);

	TSharedPtr<SHorizontalBox> RowContainer;
	TableRow->SetRowContent
	(
		SAssignNew(RowContainer, SHorizontalBox)
	);

	TSharedPtr<SWidget> RowContent = SNew(SRichTextBlock)
		.Text(this->GetHeaderText())
		.TransformPolicy(ETextTransformPolicy::ToUpper)
		.DecoratorStyleSet(&FAppStyle::Get())
		.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle");

	FMargin RowPadding = FMargin(0, 0);

	TSharedPtr<SExpanderArrow> ExpanderWidget;

	ExpanderWidget =
		SNew(SExpanderArrow, TableRow)
		.BaseIndentLevel(1);

	RowContainer->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Right)
		[
			ExpanderWidget.ToSharedRef()
		];

	RowContainer->AddSlot()
		.FillWidth(1.0)
		.VAlign(VAlign_Center)
		.Padding(RowPadding)
		[
			RowContent.ToSharedRef()
		];

	RowContainer->AddSlot()
		.AutoWidth()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Right)
		.Padding(RowPadding)
		[
			SNew(SButton)
			.ButtonStyle(FAppStyle::Get(), "SimpleButton")
			.OnClicked_Raw(this, &FCreateHeaderItem::OnAddClicked)
			.Text(FText::FromString("Add"))
			.ContentPadding(FMargin(1, 0))		
			[
				SNew(SImage)
					.Image(FAppStyle::Get().GetBrush("Icons.PlusCircle"))
					.ColorAndOpacity(FSlateColor::UseForeground())
			]
		];

	return TableRow.ToSharedRef();
}

FReply FEventHeaderItem::OnAddImplementation()
{
	if (this->GraphRef.IsValid())
	{
		this->GraphRef->CreateEvent();
	}

	return FReply::Handled();
}


#pragma region StateItem

FStateItem::FStateItem(UEdStateNode* NodePtr) : NodeRef(NodePtr) 
{
	NodePtr->Events.OnNameChanged.AddRaw(this, &FStateItem::OnNameChanged);
	NodePtr->Events.OnNodeDeleted.AddRaw(this, &FStateItem::OnNodeDeleted);
}

FStateItem::~FStateItem()
{
	if (this->NodeRef.IsValid())
	{
		this->NodeRef->Events.OnNameChanged.RemoveAll(this);
		this->NodeRef->Events.OnNodeDeleted.RemoveAll(this);
	}
}

TSharedRef<ITableRow> FStateItem::GetEntryWidget(
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	auto TableRow = SNew(STableRow<TSharedPtr<FGraphDetailsViewItem>>, OwnerTable)
		.ShowSelection(true);		

	// Make the text widget.
	TSharedPtr<SWidget> TextWidget;
	{

		SAssignNew(TextWidget, SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Right)
		[
			SNew(SExpanderArrow, TableRow)
				.BaseIndentLevel(2)
		]
		+ SHorizontalBox::Slot()
		.FillWidth(1.0)
		.Padding(0, 2)
		[
			SAssignNew(InlineText, SInlineEditableTextBlock)
				//.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
				.Text(FText::FromName(this->NodeRef->GetStateName()))
				.OnVerifyTextChanged(this, &FStateItem::OnVerifyNameTextChanged)
				.OnTextCommitted(this, &FStateItem::OnNameTextCommited)
				.IsSelected(
					TableRow, 
					&STableRow<TSharedPtr<FGraphDetailsViewItem>>::IsSelectedExclusively)
		];
	}
	
	TableRow->SetRowContent(TextWidget.ToSharedRef());

	return TableRow;
}

bool FStateItem::OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage)
{
	FName NewName(InText.ToString());

	if (NewName == this->NodeRef->GetStateName()) { return true; }

	if (this->NodeRef.IsValid())
	{
		if (auto StateGraph = Cast<UEdStateGraph>(this->NodeRef->GetGraph()))
		{
			if (StateGraph->IsStateNameAvilable(NewName))
			{
				return true;
			}
			else
			{
				OutErrorMessage = FText::FromString("State Name is in use.");
			}
		}
		else
		{
			OutErrorMessage = FText::FromString("ERROR: Graph was null?");
		}
	}
	else
	{
		OutErrorMessage = FText::FromString("ERROR: Node reference was null?");
	}

	return false;
}

void FStateItem::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	FName NewName(InText.ToString());
	this->NodeRef->SetStateName(NewName);
}

void FStateItem::OnNameChanged(FName Name)
{
	this->InlineText->SetText(FText::FromName(Name));
}

void FStateItem::OnNodeDeleted()
{
	this->GetParent().Pin()->RemoveChild(this->AsShared());
}

#pragma endregion

#pragma region StateMachineItem

FStateMachineItem::FStateMachineItem(UEdStateGraph* GraphPtr)
: GraphRef(GraphPtr)
{
	//GraphPtr->Events.OnNameChanged.AddRaw(this, &FStateMachineItem::OnNameChanged);
	GraphPtr->Events.OnGraphDeleted.AddRaw(this, &FStateMachineItem::OnNodeDeleted);
	//GraphPtr->Events.OnEventCreated.AddRaw(this, &FStateMachineItem::OnEventCreated, false);
}

FStateMachineItem::~FStateMachineItem()
{
	if (this->GraphRef.IsValid())
	{
		//this->GraphRef->Events.OnNameChanged.RemoveAll(this);
		this->GraphRef->Events.OnGraphDeleted.RemoveAll(this);
		//this->GraphRef->Events.OnEventCreated.RemoveAll(this);
	}
}

TSharedRef<ITableRow> FStateMachineItem::GetEntryWidget(
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	auto TableRow = SNew(STableRow<TSharedPtr<FGraphDetailsViewItem>>, OwnerTable)
		.ShowSelection(true);

	
	// Make the text widget.
	TSharedPtr<SWidget> TextWidget;
	{

		SAssignNew(TextWidget, SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.FillWidth(1.0)
			.Padding(0, 2)
			[
				SAssignNew(InlineText, SInlineEditableTextBlock)
					//.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
					.Text(FText::FromName(this->GraphRef->GetFName()))
					.OnVerifyTextChanged(this, &FStateMachineItem::OnVerifyNameTextChanged)
					.OnTextCommitted(this, &FStateMachineItem::OnNameTextCommited)
					.IsSelected(
						TableRow,
						&STableRow<TSharedPtr<FGraphDetailsViewItem>>::IsSelectedExclusively)
					.IsReadOnly(this->GraphRef.Get()->IsMainGraph())
			];
	}

	TableRow->SetRowContent(TextWidget.ToSharedRef());

	return TableRow;
}

void FStateMachineItem::OnEventCreated(UEdEventObject* EventObject, bool DeferRefresh)
{
	this->EventRoot->AddChild(
		MakeShareable(new FEventItem(EventObject)), DeferRefresh);
}

void FStateMachineItem::OnGraphChanged(const FEdGraphEditAction& Action)
{
	if (Action.Action & EEdGraphActionType::GRAPHACTION_AddNode)
	{
		for (auto& Node : Action.Nodes)
		{
			if (auto StateNode = Cast<UEdStateNode>(Node))
			{
				this->AddState(const_cast<UEdStateNode*>(StateNode), false);
			}
			else if (auto EventNode = Cast<UEdTransition>(Node))
			{
				//this->AddTransition(const_cast<UEdTransition*>(EventNode));
			}
		}

		//this->TreeView->RequestListRefresh();
	}
}

void FStateMachineItem::AddState(UEdStateNode* State, bool DeferRefresh)
{
	//auto Child = MakeShareable(new FStateItem(State));
	//this->AddChild(Child, DeferRefresh);
}

bool FStateMachineItem::OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage)
{
	FName NewName(InText.ToString());

	if (NewName == this->GraphRef->GetFName()) { return true; }

	if (this->GraphRef.IsValid())
	{
		if (auto BP = this->GraphRef->GetBlueprintOwner())
		{
			BP->RenameGraph(this->GraphRef.Get(), NewName);
			return true;
		}
		else
		{
			OutErrorMessage = FText::FromString("ERROR: Blueprint Owner was null?");
		}

	}
	else
	{
		OutErrorMessage = FText::FromString("ERROR: Graph reference was null?");
	}

	return false;
}

void FStateMachineItem::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	FName NewName(InText.ToString());
	//this->GraphRef->SetName(NewName);
}

void FStateMachineItem::OnNameChanged(FName Name)
{
	this->InlineText->SetText(FText::FromName(Name));
}

void FStateMachineItem::OnNodeDeleted()
{
	this->GetParent().Pin()->RemoveChild(this->AsShared());
}

#pragma endregion

#pragma region TransitionItem

FTransitionItem::FTransitionItem(UEdTransition* NodePtr) : NodeRef(NodePtr) 
{
	NodePtr->Events.OnNodeDeleted.AddRaw(this, &FTransitionItem::OnNodeDeleted);
}

FTransitionItem::~FTransitionItem()
{

}

TSharedRef<ITableRow> FTransitionItem::GetEntryWidget(
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	auto TableRow = SNew(STableRow<TSharedPtr<FGraphDetailsViewItem>>, OwnerTable)
		.ShowSelection(true);

	// Make the text widget.
	TSharedPtr<SWidget> TextWidget;
	{
		FName FromName = this->NodeRef->GetStartNode()->GetStateName();
		FName ToName = this->NodeRef->GetEndNode()->GetStateName();

		SAssignNew(TextWidget, SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Right)
			[
				SNew(SExpanderArrow, TableRow)
					.BaseIndentLevel(2)
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0)
			.Padding(0, 2)
			[
				SNew(STextBlock)
					.Text(FText::FromName(FromName))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0)
			.Padding(0, 2)
			[
				SNew(STextBlock)
					.Text(FText::FromString("->"))
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0)
			.Padding(0, 2)
			[
				SNew(STextBlock)
					.Text(FText::FromName(ToName))
			];
	}

	TableRow->SetRowContent(TextWidget.ToSharedRef());

	return TableRow;
}


void FTransitionItem::OnNodeDeleted()
{
	this->GetParent().Pin()->RemoveChild(this->AsShared());
}

#pragma endregion

#pragma region Eventitem

FEventItem::FEventItem(TWeakObjectPtr<UEdEventObject> EventObject)
	: EventReference(EventObject)
{
	EventObject->Events.OnEventRenamed.AddRaw(this, &FEventItem::OnEventRenamed);
}

TSharedRef<ITableRow> FEventItem::GetEntryWidget(
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	auto TableRow = SNew(STableRow<TSharedPtr<FGraphDetailsViewItem>>, OwnerTable)
		.ShowSelection(true);

	// Make the text widget.
	TSharedPtr<SWidget> TextWidget;
	{

		SAssignNew(TextWidget, SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Right)
			[
				SNew(SExpanderArrow, TableRow)
					.BaseIndentLevel(2)
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0)
			.Padding(0, 2)
			[
				SAssignNew(InlineText, SInlineEditableTextBlock)
				//.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
				.Text(FText::FromName(this->EventReference.Get()->GetName()))
				.OnVerifyTextChanged(this, &FEventItem::OnVerifyNameTextChanged)
				.OnTextCommitted(this, &FEventItem::OnNameTextCommited)
				.IsSelected(
					TableRow,
					&STableRow<TSharedPtr<FGraphDetailsViewItem>>::IsSelectedExclusively)
			];
	}

	TableRow->SetRowContent(TextWidget.ToSharedRef());

	return TableRow;
}

void FEventItem::OnEventRenamed(FName To)
{
	this->InlineText->SetText(FText::FromName(To));
}

bool FEventItem::OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage)
{
	FName NewName(InText.ToString());

	if (this->EventReference.IsValid())
	{
		if (NewName == this->EventReference.Get()->GetName()) { return true; }

		if (this->EventReference.Get()->GetGraph()->IsEventNameAvilable(NewName))
		{
			return true;
		}
		else
		{
			OutErrorMessage = FText::FromString("Event Name is in use.");
		}
	}
	else
	{
		OutErrorMessage = FText::FromString("ERROR: Node reference was null?");
	}

	return false;
}

void FEventItem::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	FName NewName(InText.ToString());
	this->EventReference.Get()->RenameEvent(NewName);
}

#pragma endregion

void SSubGraphDetails::Construct(const FArguments& InArgs, UEdStateGraph* GraphPtr)
{
	this->GraphRef = GraphPtr;
	this->StateRoot = MakeShareable(new FHeaderItem("States"));
	this->AliasRoot = MakeShareable(new FHeaderItem("Aliases"));
	this->EventRoot = MakeShareable(new FEventHeaderItem(GraphPtr, "Events"));
	this->StaticsRoot = MakeShareable(new FCreateHeaderItem(GraphPtr, "Statics"));

	this->TreeViewList.Add(this->StateRoot);
	this->TreeViewList.Add(this->AliasRoot);
	this->TreeViewList.Add(this->EventRoot);
	this->TreeViewList.Add(this->StaticsRoot);


	SAssignNew(TreeView, STreeView<TSharedPtr<FGraphDetailsViewItem>>)
		.ItemHeight(24)
		.TreeItemsSource(&this->TreeViewList)
		.OnGenerateRow(this, &SSubGraphDetails::OnGenerateRow, false)
		.SelectionMode(ESelectionMode::Single)
		.OnSelectionChanged(this, &SSubGraphDetails::OnSelectionChanged)
		.OnMouseButtonDoubleClick(this, &SSubGraphDetails::OnItemDoubleClicked)
		//.OnContextMenuOpening(InArgs._OnContextMenuOpening)
		.OnGetChildren(this, &SSubGraphDetails::OnGetChildrenForCategory)
		.OnItemScrolledIntoView(this, &SSubGraphDetails::OnItemScrolledIntoView)
		.OnSetExpansionRecursive(this, &SSubGraphDetails::OnSetExpansionRecursive)
		.HighlightParentNodesForSelection(true);

	this->StateRoot->SetTableView(this->TreeView);
	this->AliasRoot->SetTableView(this->TreeView);
	this->EventRoot->SetTableView(this->TreeView);
	this->StaticsRoot->SetTableView(this->TreeView);

	ChildSlot
	[
		SNew(SScrollBorder, this->TreeView.ToSharedRef())
		[
			this->TreeView.ToSharedRef()
		]
	];

	this->TreeView->SetItemExpansion(this->StateRoot, true);
	this->TreeView->SetItemExpansion(this->AliasRoot, true);
	this->TreeView->SetItemExpansion(this->EventRoot, true);
	this->TreeView->SetItemExpansion(this->StaticsRoot, true);

	this->BindEvents();
}

void SSubGraphDetails::BindEvents()
{
	this->GraphRef->Events.OnStateAdded.AddSP(this, &SSubGraphDetails::OnStateAdded);
	this->GraphRef->Events.OnEventCreated.AddSP(this, &SSubGraphDetails::OnEventAdded);
}

void SSubGraphDetails::OnStateAdded(UEdStateNode* State)
{
	this->AddState(State, false);
}

void SSubGraphDetails::OnEventAdded(UEdEventObject* Event)
{
	this->AddEvent(Event, false);
}

void SSubGraphDetails::AddState(UEdStateNode* Node, bool DeferRefresh)
{
	TSharedPtr<FGraphDetailsViewItem> Item = MakeShareable(new FStateItem(Node));

	this->StateRoot->AddChild(Item, DeferRefresh);
}

void SSubGraphDetails::AddEvent(UEdEventObject* Node, bool DeferRefresh)
{
	TSharedPtr<FGraphDetailsViewItem> Item = MakeShareable(new FEventItem(Node));

	this->EventRoot->AddChild(Item, DeferRefresh);
}

void SGraphDetailsView::Construct(
	const FArguments& InArgs,
	TSharedPtr<class FEditor> InEditor) 
{
	this->EditorPtr = InEditor;


	SAssignNew(TreeView, STreeView<TSharedPtr<FGraphDetailsViewItem>>)
		.ItemHeight(24)
		.TreeItemsSource(&this->TreeViewList)
		
		.OnGenerateRow(this, &SGraphDetailsView::OnGenerateRow, false)
		.SelectionMode(ESelectionMode::Single)
		.OnSelectionChanged(this, &SGraphDetailsView::OnSelectionChanged)
		.OnMouseButtonDoubleClick(this, &SGraphDetailsView::OnItemDoubleClicked)
		//.OnContextMenuOpening(InArgs._OnContextMenuOpening)
		.OnGetChildren(this, &SGraphDetailsView::OnGetChildrenForCategory)
		.OnItemScrolledIntoView(this, &SGraphDetailsView::OnItemScrolledIntoView)
		.OnSetExpansionRecursive(this, &SGraphDetailsView::OnSetExpansionRecursive)
		.HighlightParentNodesForSelection(true);

	// Set up the header for State Machines list.
	TSharedPtr<SHorizontalBox> RowContainer;
	{
		SAssignNew(RowContainer, SHorizontalBox);

		TSharedPtr<SWidget> RowContent = SNew(SRichTextBlock)
			.Text(FText::FromString("State Machines"))
			.TransformPolicy(ETextTransformPolicy::ToUpper)
			.DecoratorStyleSet(&FAppStyle::Get())
			.TextStyle(FAppStyle::Get(), "DetailsView.CategoryTextStyle");

		FMargin RowPadding = FMargin(0, 0);

		RowContainer->AddSlot()
			.FillWidth(1.0)
			.VAlign(VAlign_Center)
			.Padding(RowPadding)
			[
				RowContent.ToSharedRef()
			];

		RowContainer->AddSlot()
			.AutoWidth()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Right)
			.Padding(RowPadding)
			[
				SNew(SButton)
				.ButtonStyle(FAppStyle::Get(), "SimpleButton")
				.OnClicked_Raw(this, &SGraphDetailsView::OnAddStateMachine)
				.Text(FText::FromString("Add"))
				.ContentPadding(FMargin(1, 0))
				[
					SNew(SImage)
						.Image(FAppStyle::Get().GetBrush("Icons.PlusCircle"))
						.ColorAndOpacity(FSlateColor::UseForeground())
				]
			];
	}

	ChildSlot
	[
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(FilterBox, SSearchBox)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			RowContainer.ToSharedRef()
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			SNew(SScrollBorder, this->TreeView.ToSharedRef())
			[
				this->TreeView.ToSharedRef()
			]
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			SAssignNew(DetailsTabs, SWidgetSwitcher)
		]
	];

	//this->TreeView->SetItemExpansion(this->TreeListRoot, true);

	this->BindEvents(InEditor);
	this->InitView(InEditor);
}

void SGraphDetailsView::InitView(TSharedPtr<FEditor> InEditor)
{
	auto BP = InEditor->GetStateMachineBlueprintObj();
	this->InitGraphDetailsView(BP->GetMainGraph());

	for (auto SubGraph : BP->GetSubgraphs())
	{
		this->InitGraphDetailsView(SubGraph);
	}
	

	this->DetailsTabs->SetActiveWidgetIndex(0);
	this->TreeView->RequestListRefresh();
}

void SGraphDetailsView::InitGraphDetailsView(UEdStateGraph* Graph)
{
	TSharedPtr<FGraphDetailsViewItem> Item = MakeShareable(new FStateMachineItem(Graph));
	auto Widget = SNew(SSubGraphDetails, Graph);

	this->TreeViewList.Add(Item);
	Item->SetTableView(this->TreeView);
	this->TreeView->SetItemExpansion(Item, true);
	this->DetailsTabs->AddSlot(this->DetailsTabs->GetNumWidgets())[Widget];
	this->GraphToWidgetMap.Add(Graph, Widget);

	for (auto Node : Graph->GetStates())
	{
		Widget->AddState(Node, true);
	}

	for (auto Ev : Graph->GetEventList())
	{
		Widget->AddEvent(Ev, true);
	}

	//Graph->Events.OnStateAdded.AddRaw(this, &SGraphDetailsView::OnStateAdded);

	Widget->Refresh();
}

void SGraphDetailsView::OnGraphSelected(UEdStateGraph* Graph)
{
	if (this->GraphToWidgetMap.Contains(Graph))
	{
		this->DetailsTabs->SetActiveWidget(this->GraphToWidgetMap[Graph].ToSharedRef());
	}
}

FReply SGraphDetailsView::OnAddStateMachine()
{
	auto BP = this->EditorPtr.Pin()->GetStateMachineBlueprintObj();
	auto NewGraph = BP->AddSubGraph();
	this->InitGraphDetailsView(NewGraph);

	return FReply::Handled();
}

void SGraphDetailsView::OnStateAdded(UEdStateNode* Node)
{
	auto Graph = Cast<UEdStateGraph>(Node->GetGraph());

	if (this->GraphToWidgetMap.Contains(Graph))
	{
		this->GraphToWidgetMap[Graph]->AddState(Node, false);
	}
}

void SGraphDetailsView::OnSelectionChanged(
	TSharedPtr<FGraphDetailsViewItem> SelectedItem,
	ESelectInfo::Type SelectInfo)
{
	if (SelectedItem.IsValid())
	{ 
		SelectedItem->Select();
	}	
}

TSharedRef<ITableRow> SGraphDetailsView::OnGenerateRow(
	TSharedPtr<FGraphDetailsViewItem> InItem,
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	return InItem->GetEntryWidget(OwnerTable, bIsReadOnly);
}

void SGraphDetailsView::AddReferencedObjects(FReferenceCollector& Collector) {

}

void OnGraphSelected(UEdStateGraph* Graph)
{

}

void SGraphDetailsView::BindEvents(TSharedPtr<class FEditor> InEditor) {
	//InEditor->CommandEvents.NewStateEvent.AddRaw(this, &SGraphDetailsView::AddState);

	if (auto BPObj = InEditor->GetStateMachineBlueprintObj()) {		
		auto Graph = BPObj->GetMainGraph();
		check(Graph);

		BPObj->Events.OnGraphSelected.AddSP(this, &SGraphDetailsView::OnGraphSelected);
	}
}

void SGraphDetailsView::OnGetChildrenForCategory(
	TSharedPtr<FGraphDetailsViewItem> InItem, 
	TArray< TSharedPtr<FGraphDetailsViewItem> >& OutChildren)
{
	InItem->GetChildren(OutChildren);
}

void FGraphDetailsViewItem::GetChildren(TArray< TSharedPtr<FGraphDetailsViewItem> >& OutChildren) const
{
	for (auto& Child : this->Children)
	{
		OutChildren.Add(Child);
	}
}

void SSubGraphDetails::OnGetChildrenForCategory(
	TSharedPtr<FGraphDetailsViewItem> InItem,
	TArray< TSharedPtr<FGraphDetailsViewItem> >& OutChildren)
{
	InItem->GetChildren(OutChildren);
}

TSharedRef<ITableRow> SSubGraphDetails::OnGenerateRow(
	TSharedPtr<FGraphDetailsViewItem> InItem,
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	return InItem->GetEntryWidget(OwnerTable, bIsReadOnly);
}

void SSubGraphDetails::OnSelectionChanged(
	TSharedPtr<FGraphDetailsViewItem> SelectedItem,
	ESelectInfo::Type SelectInfo)
{
	if (SelectedItem.IsValid())
	{
		SelectedItem->Select();
	}
}

void SSubGraphDetails::AddReferencedObjects(FReferenceCollector& Collector)
{

}

#undef LOCTEXT_NAMESPACE