#include "StateMachine/Widgets/GraphDetailsView.h"

#define LOCTEXT_NAMESPACE "PSM"

void SGraphDetailsView::Construct(
	const FArguments& InArgs,
	TSharedPtr<class FEditor> InBlueprintEditor) 
{
	auto WidgetName = FName("SGraphDetailsView");

	ChildSlot
	[
		SNew(STextBlock)
		.Text(FText::Format(LOCTEXT("GraphView", "MyWidgetName: {0}"), FText::FromName(WidgetName)))
	];
}


void SGraphDetailsView::AddReferencedObjects(FReferenceCollector& Collector) {

}

#undef LOCTEXT_NAMESPACE