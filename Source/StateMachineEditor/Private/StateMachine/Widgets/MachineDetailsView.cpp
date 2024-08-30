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
	];

	this->BindEvents(InEditor);
}

void SMachineDetailsView::BindEvents(TSharedPtr<class FEditor> InEditor)
{
	if (auto BPObj = InEditor->GetStateMachineBlueprintObj())
	{
		auto Graph = BPObj->GetMainGraph();

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
		Inspector->ShowDetailsForSingleObject(nullptr);
	}
}


#undef LOCTEXT_NAMESPACE