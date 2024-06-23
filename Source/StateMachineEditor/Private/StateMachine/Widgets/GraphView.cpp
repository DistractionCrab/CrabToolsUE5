#include "StateMachine/Widgets/GraphView.h"
#include "StateMachine/Widgets/Nodes/EdStateNodeFactory.h"
#include "StateMachine/StateMachineBlueprint.h"

#include "Widgets/Layout/SWidgetSwitcher.h"

#define LOCTEXT_NAMESPACE "SGraphView"

void SGraphView::Construct(
	const FArguments& InArgs,
	TSharedPtr<class FEditor> InBlueprintEditor)
{
	FGraphAppearanceInfo AppInfo;
	
	if (auto BP = Cast<UStateMachineBlueprint>(InBlueprintEditor->GetBlueprintObj())) {		
		auto Graph = BP->GetMainGraph();
		
		SGraphEditor::FGraphEditorEvents InEvents;

		InEvents.OnSelectionChanged.BindRaw(this, &SGraphView::OnSelectionChanged);

		ChildSlot
		[
			SAssignNew(TabsWidget, SWidgetSwitcher)
			+ SWidgetSwitcher::Slot()
			[
				SAssignNew(GraphEditor, SGraphEditor)
					.IsEditable(true)
					.Appearance(AppInfo)
					.GraphToEdit(Graph)
					.GraphEvents(InEvents)
			]
		];

		this->GraphToEditorMap.Add(Graph, this->GraphEditor);
		this->TabsWidget->SetActiveWidget(this->GraphEditor.ToSharedRef());

		this->BindEvents(BP);
	}
}

void SGraphView::AddReferencedObjects(FReferenceCollector& Collector)
{

}

void SGraphView::BindEvents(UStateMachineBlueprint* Blueprint)
{
	Blueprint->Events.OnGraphSelected.AddSP(this, &SGraphView::OnGraphSelected);
}

void SGraphView::OnGraphSelected(UEdStateGraph* Graph)
{
	
}

void SGraphView::OnSelectionChanged(const TSet<UObject*>& NewSelection)
{
	if (auto Graph = Cast<UEdStateGraph>(this->GraphEditor->GetCurrentGraph()))
	{
		TArray<UEdGraphNode*> Nodes;

		for (const auto& Elem : NewSelection) 
		{ 
			if (auto CastElem = Cast<UEdGraphNode>(Elem))
			{
				Nodes.Add(CastElem);
			}			
		}
		
		Graph->Events.OnNodeSelected.Broadcast(Nodes);
	}
}

FReply SGraphView::OnKeyDown(const FGeometry& Geo, const FKeyEvent& Event)
{
	if (Event.GetKey() == EKeys::Delete || Event.GetKey() == EKeys::BackSpace)
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