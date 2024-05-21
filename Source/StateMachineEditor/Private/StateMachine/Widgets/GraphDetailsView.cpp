#include "StateMachine/Widgets/GraphDetailsView.h"

#include "Widgets/SWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SScrollBorder.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

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

TSharedRef<ITableRow> FGraphDetailsViewItem::GetEntryWidget(
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	TSharedPtr<IToolTip> SectionToolTip;
	TSharedPtr<STableRow<TSharedPtr<FGraphDetailsViewItem>>> TableRow;

	TableRow = SNew(SCategoryHeaderTableRow<TSharedPtr<FGraphDetailsViewItem>>, OwnerTable)
		.ToolTip(SectionToolTip);
	TableRow->SetRowContent(SNew(STextBlock).Text(FText::FromString("NULL WIDGET")));

	return TableRow.ToSharedRef();
}


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

FStateItem::FStateItem(UEdStateNode* NodePtr) : NodeRef(NodePtr) 
{
	NodePtr->Events.OnNameChanged.AddRaw(this, &FStateItem::OnNameChanged);
}

TSharedRef<ITableRow> FStateItem::GetEntryWidget(
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	
	// Make the text widget.
	TSharedPtr<SBorder> TextWidget;
	{
		SAssignNew(TextWidget, SBorder)
		.Padding(15, 5)
		[
			SAssignNew(InlineText, SInlineEditableTextBlock)
				.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
				.Text(FText::FromName(this->NodeRef->GetStateName()))
				//.OnVerifyTextChanged(this, &SEdStateNode::OnVerifyNameTextChanged)
				.OnTextCommitted(this, &FStateItem::OnNameTextCommited)
				//.IsReadOnly(this, &SEdStateNode::IsNameReadOnly)
				.IsSelected(this, &FStateItem::IsSelected)
		];
	}

	auto TableRow = SNew(STableRow<TSharedPtr<FGraphDetailsViewItem>>, OwnerTable)
		//.OnDragDetected(this, &SGraphActionMenu::OnItemDragDetected)
		//.bAllowPreselectedItemActivation(bAllowPreselectedItemActivation);
		.ShowSelection(true);
	
	TableRow->SetRowContent(TextWidget.ToSharedRef());

	return TableRow;
}

bool FStateItem::IsSelected() const
{
	return true;
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

void SGraphDetailsView::Construct(
	const FArguments& InArgs,
	TSharedPtr<class FEditor> InEditor) 
{
	this->StateRoot = MakeShareable(new FHeaderItem("States"));
	this->StateList.Add(this->StateRoot);
	this->StateList.Add(MakeShareable(new FHeaderItem("Aliases")));
	this->StateList.Add(MakeShareable(new FHeaderItem("Events")));
	this->StateList.Add(MakeShareable(new FHeaderItem("Statics")));
	

	SAssignNew(StateListWidget, STreeView<TSharedPtr<FGraphDetailsViewItem>>)
		.ItemHeight(24)
		.TreeItemsSource(&this->StateList)
		.OnGenerateRow(this, &SGraphDetailsView::OnGenerateRow, false)
		.SelectionMode(ESelectionMode::Single)
		.OnSelectionChanged(this, &SGraphDetailsView::OnItemSelected)
		.OnMouseButtonDoubleClick(this, &SGraphDetailsView::OnItemDoubleClicked)
		//.OnContextMenuOpening(InArgs._OnContextMenuOpening)
		.OnGetChildren(this, &SGraphDetailsView::OnGetChildrenForCategory)
		.OnItemScrolledIntoView(this, &SGraphDetailsView::OnItemScrolledIntoView)
		.OnSetExpansionRecursive(this, &SGraphDetailsView::OnSetExpansionRecursive)
		.HighlightParentNodesForSelection(true);

	

	ChildSlot
	[
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(FilterBox, SSearchBox)
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			SNew(SScrollBorder, this->StateListWidget.ToSharedRef())
			[
				this->StateListWidget.ToSharedRef()
			]
		]
	];


	this->BindEvents(InEditor);
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

void SGraphDetailsView::BindEvents(TSharedPtr<class FEditor> InEditor) {
	//InEditor->CommandEvents.NewStateEvent.AddRaw(this, &SGraphDetailsView::AddState);

	if (auto BPObj = InEditor->GetStateMachineBlueprintObj()) {		
		FOnGraphChanged::FDelegate f;
		f.BindRaw(this, &SGraphDetailsView::OnGraphChanged);

		
		BPObj->StateMachineGraph()->AddOnGraphChangedHandler(f);
	}
}

void SGraphDetailsView::OnGraphChanged(const FEdGraphEditAction& Action) {
	if (Action.Action & EEdGraphActionType::GRAPHACTION_AddNode) {
		for (auto& Node : Action.Nodes) {
			if (auto CastNode = Cast<UEdStateNode>(Node)) {
				this->AddState(const_cast<UEdStateNode*>(CastNode));
			}			
		}
	}
}

void SGraphDetailsView::AddState(UEdStateNode* Node)
{
	UE_LOG(LogTemp, Warning, TEXT("Attemping to add node to view."));
	this->StateRoot->AddChild(MakeShareable(new FStateItem(Node)));
	this->StateListWidget->RequestListRefresh();
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

#undef LOCTEXT_NAMESPACE