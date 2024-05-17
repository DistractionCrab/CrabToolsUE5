#include "StateMachine/Widgets/GraphView.h"
#include "StateMachine/Widgets/Nodes/EdStateNodeFactory.h"
#include "StateMachine/StateMachineBlueprint.h"

#define LOCTEXT_NAMESPACE "SGraphView"

void SGraphView::Construct(
	const FArguments& InArgs,
	TSharedPtr<class FEditor> InBlueprintEditor) 
{
	//SGraphEditor::Construct(InArgs);
	FGraphAppearanceInfo AppInfo;
	//AppInfo.CornerText = LOCTEXT("AppearanceCornerText_StateMachine", "State Machine Graph");
	
	if (auto BP = Cast<UStateMachineBlueprint>(InBlueprintEditor->GetBlueprintObj())) {		
		
		SGraphEditor::FGraphEditorEvents InEvents;

		/*
		InEvents.OnSelectionChanged
			= SGraphEditor::FOnSelectionChanged::CreateLambda([] (
				const TSet<class UObject*>& NewSelection)
				{
					UE_LOG(LogTemp, Warning, TEXT("Selected objects?"));
				});
		*/

		InEvents.OnSelectionChanged.BindRaw(this, &SGraphView::OnSelectionChanged);

		ChildSlot
		[
			SAssignNew(GraphEditor, SGraphEditor)
				.IsEditable(true)
				.Appearance(AppInfo)
				.GraphToEdit(BP->StateMachineGraph())
				.GraphEvents(InEvents)			
		];
	}
}

void SGraphView::AddReferencedObjects(FReferenceCollector& Collector) {

}

void SGraphView::OnSelectionChanged(const TSet<UObject*>& NewSelection)
{
	if (auto Graph = Cast<UEdStateGraph>(this->GraphEditor->GetCurrentGraph()))
	{
		TArray<UEdStateNode*> Nodes;

		for (const auto& Elem : NewSelection) 
		{ 
			if (UEdStateNode* CastElem = Cast<UEdStateNode>(Elem))
			{
				Nodes.Add(CastElem);
			}			
		}
		
		Graph->Events.OnNodeSelected.Broadcast(Nodes);
	}
}

FReply SGraphView::OnKeyDown(const FGeometry& Geo, const FKeyEvent& Event)
{
	// If delete or backspace has been pressed has been pressed
	if (Event.GetKeyCode() == 46 || Event.GetKeyCode() == 8)
	{
		if (auto Graph = this->GraphEditor->GetCurrentGraph())
		{
			for (auto& Node : this->GraphEditor->GetSelectedNodes())
			{
				auto CastNode = Cast<UEdGraphNode>(Node);
				if (CastNode && CastNode->CanUserDeleteNode())
				{
					Graph->RemoveNode(CastNode);
				}
			}
			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

#undef LOCTEXT_NAMESPACE