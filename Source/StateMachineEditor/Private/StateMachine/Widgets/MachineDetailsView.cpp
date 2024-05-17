#include "StateMachine/Widgets/MachineDetailsView.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "SKismetInspector.h"
#include "GraphEditAction.h"

#define LOCTEXT_NAMESPACE "PSM"

void SMachineDetailsView::Construct(
	const FArguments& InArgs,
	TSharedPtr<class FEditor> InEditor) 
{
	auto WidgetName = FName("SMachineDetailsView");

	ChildSlot
	[
		SAssignNew(Inspector, SKismetInspector)
		//SNew(STextBlock)
		//.Text(FText::Format(LOCTEXT("GraphView", "MyWidgetName: {0}"), FText::FromName(WidgetName)))
	];

	this->BindEvents(InEditor);
}

void SMachineDetailsView::BindEvents(TSharedPtr<class FEditor> InEditor)
{
	//InEditor->CommandEvents.NewStateEvent.AddRaw(this, &SGraphDetailsView::AddState);

	if (auto BPObj = InEditor->GetStateMachineBlueprintObj())
	{
		FOnGraphChanged::FDelegate f;
		f.BindRaw(this, &SMachineDetailsView::OnGraphChanged);

		BPObj->StateMachineGraph()->AddOnGraphChangedHandler(f);
		BPObj->StateMachineGraph()->Events.OnNodeSelected.AddRaw(
			this, 
			&SMachineDetailsView::OnSelectionChanged);
	}
}

void SMachineDetailsView::AddReferencedObjects(FReferenceCollector& Collector)
{

}

void SMachineDetailsView::OnSelectionChanged(TArray<class UEdStateNode*>& SelectedNodes)
{
	if (SelectedNodes.Num() == 1)
	{
		Inspector->ShowDetailsForSingleObject(SelectedNodes[0]);
	}
}

void SMachineDetailsView::OnGraphChanged(const FEdGraphEditAction& Action)
{
	if (Action.Action == EEdGraphActionType::GRAPHACTION_SelectNode)
	{
		if (Action.Nodes.Num() == 1)
		{
			for (auto Node : Action.Nodes)
			{
				// Need to static cast due to limitations of this interface.
				UObject* CastNode = const_cast<UEdGraphNode*>(Node);
				Inspector->ShowDetailsForSingleObject(CastNode);
			}
		}
	}
	else if (Action.Action == EEdGraphActionType::GRAPHACTION_AddNode)
	{
		for (auto Node : Action.Nodes)
		{
			UObject* CastNode = const_cast<UEdGraphNode*>(Node);
			Inspector->ShowDetailsForSingleObject(CastNode);
		}
	}
	else
	{

	}
}

#undef LOCTEXT_NAMESPACE