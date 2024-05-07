#include "StateMachine/Schema/GraphSchema.h"
#include "StateMachine/StateMachineBlueprint.h"


UEdGraphNode* FSMSchemaAction_NewNode::PerformAction(
	class UEdGraph* ParentGraph, 
	UEdGraphPin* FromPin, 
	const FVector2D Location, 
	bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = nullptr;

	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(
			LOCTEXT("StateMachineEditorNewNode", "State Machine Editor: New Node"));

		ParentGraph->Modify();
		if (FromPin != nullptr)
			FromPin->Modify();

		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		NodeTemplate->GenericGraphNode->SetFlags(RF_Transactional);
		NodeTemplate->SetFlags(RF_Transactional);

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}



#pragma region Schema

UStateMachineSchema::UStateMachineSchema(
	const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}


void UStateMachineSchema::BackwardCompatibilityNodeConversion(UEdGraph* Graph, bool bOnlySafeChanges) const
{
	if (Graph)
	{
		if (UStateMachineBlueprint* ProcStateMachineBlueprint = Cast<UStateMachineBlueprint>(Graph->GetOuter()))
		{
			const int32 ProcStateMachineBPVersion = ProcStateMachineBlueprint->GetLinkerCustomVersion(FFortniteMainBranchObjectVersion::GUID);
		}
	}

	Super::BackwardCompatibilityNodeConversion(Graph, bOnlySafeChanges);
}

EGraphType YStateMachineSchema::GetGraphType(const UEdGraph* TestEdGraph) const
{
	return GT_StateMachine;
}



void UAssetGraphSchema_GenericGraph::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	//UGenericGraph* Graph = CastChecked<UGenericGraph>(ContextMenuBuilder.CurrentGraph->GetOuter());

	/*
	if (Graph->NodeType == nullptr)
	{
		return;
	}
	*/

	const bool bNoParent = (ContextMenuBuilder.FromPin == NULL);

	const FText AddToolTip = LOCTEXT("NewStateGraphNodeTooltip", "Add node here");

	TSet<TSubclassOf<UGenericGraphNode> > Visited;

	/*
	FText Desc = Graph->NodeType.GetDefaultObject()->ContextMenuName;

	if (Desc.IsEmpty())
	{
		FString Title = Graph->NodeType->GetName();
		Title.RemoveFromEnd("_C");
		Desc = FText::FromString(Title);
	}

	if (!Graph->NodeType->HasAnyClassFlags(CLASS_Abstract))
	{
		TSharedPtr<FAssetSchemaAction_GenericGraph_NewNode> NewNodeAction(new FAssetSchemaAction_GenericGraph_NewNode(LOCTEXT("GenericGraphNodeAction", "Generic Graph Node"), Desc, AddToolTip, 0));
		NewNodeAction->NodeTemplate = NewObject<UEdNode_GenericGraphNode>(ContextMenuBuilder.OwnerOfTemporaries);
		NewNodeAction->NodeTemplate->GenericGraphNode = NewObject<UGenericGraphNode>(NewNodeAction->NodeTemplate, Graph->NodeType);
		NewNodeAction->NodeTemplate->GenericGraphNode->Graph = Graph;
		ContextMenuBuilder.AddAction(NewNodeAction);

		Visited.Add(Graph->NodeType);
	}*/

	for (TObjectIterator<UClass> It; It; ++It)
	{
		bool c1 = (*It)->IsChildOf(UStateNode::StaticClass());
		bool c2 = !It->HasAnyClassFlags(CLASS_Abstract);
		bool c3 = !Visited.Contains(*It);

		if ( c1 && c2 && c3 )
		{
			TSubclassOf<UStateNode> NodeType = *It;

			if (It->GetName().StartsWith("REINST") || It->GetName().StartsWith("SKEL"))
				continue;

			//if (!Graph->GetClass()->IsChildOf(NodeType.GetDefaultObject()->CompatibleGraphType))
			//	continue;

			Desc = NodeType.GetDefaultObject()->ContextMenuName;

			if (Desc.IsEmpty())
			{
				FString Title = NodeType->GetName();
				Title.RemoveFromEnd("_C");
				Desc = FText::FromString(Title);
			}

			TSharedPtr<FAssetSchemaAction_GenericGraph_NewNode> Action(
				new FAssetSchemaAction_GenericGraph_NewNode(
					LOCTEXT("StateMAchineGraphNodeAction", "State Machine Graph Node"), Desc, AddToolTip, 0));
			Action->NodeTemplate = NewObject<UEdStateNode>(ContextMenuBuilder.OwnerOfTemporaries);
			Action->NodeTemplate->SetNodeTemplate(NewObject<UGenericGraphNode>(Action->NodeTemplate, NodeType));
			//Action->NodeTemplate->GenericGraphNode->Graph = Graph;
			ContextMenuBuilder.AddAction(Action);

			Visited.Add(NodeType);
		}
	}
}

#pragma endregion