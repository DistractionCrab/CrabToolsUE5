#include "StateMachine/Widgets/GraphDetailsView.h"

#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "PSM"

void SGraphDetailsView::Construct(
	const FArguments& InArgs,
	TSharedPtr<class FEditor> InBlueprintEditor) 
{
	ChildSlot
	[
		SNew(SVerticalBox)
		// Slot for the State list.
		+ SVerticalBox::Slot()
			.VAlign(VAlign_Fill) [
			SNew(SVerticalBox)
			+ SVerticalBox::Slot() [
				SNew(STextBlock)
					.Text(LOCTEXT("GraphDetaulsView", "States"))
			]
			+ SVerticalBox::Slot()[
				SNew(SScrollBox) 
				+ SScrollBox::Slot() [
					SAssignNew(StateListWidget, SVerticalBox)
				]
			]
			
		]
	];

	this->BindEvents(InBlueprintEditor);
}


void SGraphDetailsView::AddReferencedObjects(FReferenceCollector& Collector) {

}

void SGraphDetailsView::BindEvents(TSharedPtr<class FEditor> InEditor) {
	InEditor->CommandEvents.NewStateEvent.AddRaw(this, &SGraphDetailsView::AddState);
}

void SGraphDetailsView::AddState(class UEdStateNode* Node) {

}

#undef LOCTEXT_NAMESPACE