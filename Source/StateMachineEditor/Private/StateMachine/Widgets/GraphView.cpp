#include "StateMachine/Widgets/GraphView.h"

#define LOCTEXT_NAMESPACE "PSM"

void SGraphView::Construct(
	const FArguments& InArgs,
	TSharedPtr<class FProcStateMachineBlueprintEditor> InBlueprintEditor) 
{

	auto WidgetName = FName("SGraphView");

	ChildSlot
	[
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