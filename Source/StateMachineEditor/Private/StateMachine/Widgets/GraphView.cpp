#include "StateMachine/Widgets/GraphView.h"
#include "StateMachine/StateMachineBlueprint.h"

#define LOCTEXT_NAMESPACE "PSM"

void SGraphView::Construct(
	const FArguments& InArgs,
	TSharedPtr<class FEditor> InBlueprintEditor) 
{

	auto WidgetName = FName("SGraphView");

	FGraphAppearanceInfo AppInfo;
	AppInfo.CornerText = LOCTEXT("AppearanceCornerText_StateMachine", "State Machine Graph");

	
	if (UStateMachineBlueprint* BP = Cast<UStateMachineBlueprint>(InBlueprintEditor->GetBlueprintObj())) {		
		ChildSlot
			[
				SAssignNew(MainGraph, SGraphEditor)
					.IsEditable(true)
					.Appearance(AppInfo)
				  .GraphToEdit(BP->SMGraph())
				//SNew(STextBlock)
				//	.Text(FText::Format(LOCTEXT("GraphView", "MyWidgetName: {0}"), FText::FromName(WidgetName)))
			];
	}


	
}

void SGraphView::AddReferencedObjects(FReferenceCollector& Collector) {

}


FReply SGraphView::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) {
	return FReply::Unhandled();
}

#undef LOCTEXT_NAMESPACE