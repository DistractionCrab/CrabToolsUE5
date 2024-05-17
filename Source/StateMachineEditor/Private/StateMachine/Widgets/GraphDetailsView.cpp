#include "StateMachine/Widgets/GraphDetailsView.h"

#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SScrollBorder.h"

#include "GraphEditAction.h"
#include "StateMachine/StateMachineBlueprint.h"

#define LOCTEXT_NAMESPACE "PSM"

class FStateListEntry {};
class FEventListEntry {};
class FAliasListEntry {};



void SGraphDetailsView::Construct(
	const FArguments& InArgs,
	TSharedPtr<class FEditor> InEditor) 
{
	for (int i = 0; i < 50; ++i)
	{
		this->StateList.Add(MakeShareable(new FStateListEntry));
	}

	SAssignNew(StateListWidget, STreeView<TSharedPtr<FStateListEntry>>)
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
	TSharedPtr<FStateListEntry> InItem,
	const TSharedRef<STableViewBase>& OwnerTable,
	bool bIsReadOnly)
{
	return SNew(STableRow<TSharedPtr<FStateListEntry>>, OwnerTable)
		[
			SNew(STextBlock)
				.Text(FText::FromString("Waffles"))
		];
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
	if (Action.Action == EEdGraphActionType::GRAPHACTION_AddNode) {
		for (const auto& Node : Action.Nodes) {
			if (auto CastNode = Cast<UEdStateNode>(Node)) {
				this->AddState(CastNode);
			}			
		}
	}
}

void SGraphDetailsView::AddState(const UEdStateNode* Node) {

}

FText SGraphDetailsView::OnGetSectionTitle(int32 InSectionID) {
	return FText::FromString("Example Title");
}

TSharedRef<SWidget> SGraphDetailsView::OnGetSectionWidget(TSharedRef<SWidget> RowWidget, int32 InSectionID) {
	return RowWidget;
}

#undef LOCTEXT_NAMESPACE