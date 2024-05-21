#include "StateMachine/Widgets/Nodes/SEdEventGraphPin.h"
#include "StateMachine/EdGraph/StateMachineGraphDragConnection.h"

void SEdEventGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	this->SetCursor(EMouseCursor::Default);

	bShowLabel = true;

	GraphPinObj = InPin;
	check(GraphPinObj != nullptr);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SEdEventGraphPin::GetPinBorder)
		.BorderBackgroundColor(this, &SEdEventGraphPin::GetPinColor)
		.OnMouseButtonDown(this, &SEdEventGraphPin::OnPinMouseDown)
		.Cursor(this, &SEdEventGraphPin::GetPinCursor)
		.Padding(FMargin(5.0f))
	);
}


TSharedRef<FDragDropOperation> SEdEventGraphPin::SpawnPinDragEvent(
	const TSharedRef<class SGraphPanel>& InGraphPanel, 
	const TArray< TSharedRef<SGraphPin> >& InStartingPins)
{
	FStateMachineGraphDragConnection::FDraggedPinTable PinHandles;
	PinHandles.Reserve(InStartingPins.Num());
	// since the graph can be refreshed and pins can be reconstructed/replaced 
	// behind the scenes, the DragDropOperation holds onto FGraphPinHandles 
	// instead of direct widgets/graph-pins
	for (const TSharedRef<SGraphPin>& PinWidget : InStartingPins)
	{
		PinHandles.Add(PinWidget->GetPinObj());
	}

	return FStateMachineGraphDragConnection::New(InGraphPanel, PinHandles);
}