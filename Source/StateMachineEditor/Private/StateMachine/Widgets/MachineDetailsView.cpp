#include "StateMachine/Widgets/MachineDetailsView.h"

#define LOCTEXT_NAMESPACE "PSM"

void SMachineDetailsView::Construct(
	const FArguments& InArgs,
	TSharedPtr<class FEditor> InBlueprintEditor) 
{
	auto WidgetName = FName("SMachineDetailsView");

	ChildSlot
	[
		SNew(STextBlock)
		.Text(FText::Format(LOCTEXT("GraphView", "MyWidgetName: {0}"), FText::FromName(WidgetName)))
	];
}


void SMachineDetailsView::AddReferencedObjects(FReferenceCollector& Collector) {

}

#undef LOCTEXT_NAMESPACE