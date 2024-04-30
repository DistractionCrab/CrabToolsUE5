#include "StateMachine/Widgets/GraphView.h"

#define LOCTEXT_NAMESPACE "PSM"

void SGraphView::Construct(
	const FArguments& InArgs,
	TSharedPtr<class FEditor> InBlueprintEditor) 
{

	auto WidgetName = FName("SGraphView");

	FGraphAppearanceInfo AppInfo;
	AppInfo.CornerText = LOCTEXT("AppearanceCornerText_StateMachine", "State Machine Graph");

	ChildSlot
	[
		//SAssignNew(MainGraph, SGraphEditor)
		//	.IsEditable(true)
		//	.Appearance(AppInfo)
		//  .GraphToEdit(nullptr)
		SNew(STextBlock)
			.Text(FText::Format(LOCTEXT("GraphView", "MyWidgetName: {0}"), FText::FromName(WidgetName)))
	];
}

void SGraphView::AddReferencedObjects(FReferenceCollector& Collector) {

}


FReply SGraphView::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) {
	return FReply::Unhandled();
}

#undef LOCTEXT_NAMESPACE