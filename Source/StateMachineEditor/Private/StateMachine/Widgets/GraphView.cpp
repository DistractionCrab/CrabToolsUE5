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
		this->BlueprintRef = BP;
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

void SGraphView::AddGraphToEditor(UEdGraph* Graph)
{
	FGraphAppearanceInfo AppInfo;
	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged.BindRaw(this, &SGraphView::OnSelectionChanged);

	TSharedPtr<SGraphEditor> NewEditor = SNew(SGraphEditor)
		.IsEditable(true)
		.Appearance(AppInfo)
		.GraphToEdit(Graph)
		.GraphEvents(InEvents);

	this->TabsWidget->AddSlot(this->TabsWidget->GetNumWidgets())[NewEditor.ToSharedRef()];
	this->GraphToEditorMap.Add(Graph, NewEditor);
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
	if (!this->GraphToEditorMap.Contains(Graph))
	{
		this->AddGraphToEditor(Graph);
	}

	auto Widget = this->GraphToEditorMap[Graph];
	this->TabsWidget->SetActiveWidget(Widget.ToSharedRef());
	this->GraphEditor = Widget;
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

		// If a singular node was selected, send that for inspection also.
		if (Nodes.Num() == 1)
		{
			this->BlueprintRef->Events.OnObjectInspected.Broadcast(Nodes[0]);
		}
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