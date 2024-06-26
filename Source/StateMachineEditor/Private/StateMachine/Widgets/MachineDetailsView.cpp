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
		auto Graph = BPObj->GetMainGraph();

		//FOnGraphChanged::FDelegate f;
		//f.BindRaw(this, &SMachineDetailsView::OnGraphChanged);

		//Graph->AddOnGraphChangedHandler(f);
		/*Graph->Events.OnNodeSelected.AddRaw(
			this, 
			&SMachineDetailsView::OnSelectionChanged);*/
		BPObj->Events.OnObjectInspected.AddSP(this, &SMachineDetailsView::InspectObject);
	}
}

void SMachineDetailsView::InspectObject(UObject* Obj)
{
	Inspector->ShowDetailsForSingleObject(Obj);
}

void SMachineDetailsView::AddReferencedObjects(FReferenceCollector& Collector)
{

}

void SMachineDetailsView::OnSelectionChanged(TArray<class UEdGraphNode*>& SelectedNodes)
{
	if (SelectedNodes.Num() == 1 && SelectedNodes[0])
	{
		Inspector->ShowDetailsForSingleObject(SelectedNodes[0]);
	}
	else
	{
		//TArray<UObject*> Objs;
		//this->Inspector->ShowDetailsForObjects(Objs);
		Inspector->ShowDetailsForSingleObject(nullptr);
	}
}

void SMachineDetailsView::OnGraphChanged(const FEdGraphEditAction& Action)
{
	if (Action.Action & EEdGraphActionType::GRAPHACTION_SelectNode)
	{
		if (Action.Nodes.Num() == 1)
		{
			for (auto Node : Action.Nodes)
			{
				// Need to static cast due to limitations of this interface.
				UObject* CastNode = const_cast<UEdGraphNode*>(Node);
				//Inspector->ShowDetailsForSingleObject(CastNode);
			}
		}
	}
	else if (Action.Action & EEdGraphActionType::GRAPHACTION_AddNode)
	{
		for (auto Node : Action.Nodes)
		{
			UObject* CastNode = const_cast<UEdGraphNode*>(Node);
			//Inspector->ShowDetailsForSingleObject(CastNode);
		}
	}
	else
	{

	}
}

#undef LOCTEXT_NAMESPACE