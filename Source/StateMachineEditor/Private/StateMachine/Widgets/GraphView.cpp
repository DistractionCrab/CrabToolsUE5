#include "StateMachine/Widgets/GraphView.h"
#include "StateMachine/Widgets/Nodes/EdStateNodeFactory.h"
#include "StateMachine/StateMachineBlueprint.h"

#define LOCTEXT_NAMESPACE "PSM"

void SGraphView::Construct(
	const FArguments& InArgs,
	TSharedPtr<class FEditor> InBlueprintEditor) 
{
	//SGraphEditor::Construct(InArgs);
	FGraphAppearanceInfo AppInfo;
	//AppInfo.CornerText = LOCTEXT("AppearanceCornerText_StateMachine", "State Machine Graph");
	
	if (UStateMachineBlueprint* BP = Cast<UStateMachineBlueprint>(InBlueprintEditor->GetBlueprintObj())) {		
		
		ChildSlot
		[
			SAssignNew(GraphEditor, SGraphEditor)
				.IsEditable(true)
				.Appearance(AppInfo)
				.GraphToEdit(BP->SMGraph())
			
		];
	}
}

void SGraphView::AddReferencedObjects(FReferenceCollector& Collector) {

}

#undef LOCTEXT_NAMESPACE