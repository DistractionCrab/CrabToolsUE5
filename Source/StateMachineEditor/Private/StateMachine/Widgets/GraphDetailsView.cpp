#include "StateMachine/Widgets/GraphDetailsView.h"

#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SScrollBox.h"

#include "GraphEditAction.h"
#include "StateMachine/StateMachineBlueprint.h"

#define LOCTEXT_NAMESPACE "PSM"

/* Helper function used to generate the different sections of the Graph View*/
TSharedRef<class SVerticalBox> GenerateListWidgets(
	TSharedPtr<class SVerticalBox>& RefWidget,
	FString SectionHeader)
{

	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		[
			SNew(STextBlock)
				.Text(FText::Format(
					LOCTEXT("GraphDetaulsView", "{0}"), 
					FText::FromString(SectionHeader)))
		]
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			SNew(SScrollBox)
				+ SScrollBox::Slot()
				[
					SAssignNew(RefWidget, SVerticalBox)
				]
		];
}

void SGraphDetailsView::Construct(
	const FArguments& InArgs,
	TSharedPtr<class FEditor> InEditor) 
{

	ChildSlot
	[
		SAssignNew(GraphActionMenu, SGraphActionMenu, false)
	];


	this->BindEvents(InEditor);
}



void SGraphDetailsView::AddReferencedObjects(FReferenceCollector& Collector) {

}

void SGraphDetailsView::BindEvents(TSharedPtr<class FEditor> InEditor) {
	//InEditor->CommandEvents.NewStateEvent.AddRaw(this, &SGraphDetailsView::AddState);

	if (auto BPObj = InEditor->GetStateMachineBlueprintObj()) {		
		FOnGraphChanged::FDelegate f;
		f.BindRaw(this, &SGraphDetailsView::OnGraphChanged);

		
		BPObj->SMGraph()->AddOnGraphChangedHandler(f);
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