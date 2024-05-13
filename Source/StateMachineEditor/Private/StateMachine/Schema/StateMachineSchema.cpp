#include "StateMachine/Schema/StateMachineSchema.h"
#include "StateMachine/StateMachineBlueprint.h"

#define LOCTEXT_NAMESPACE "StateMachineSchema"

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

		//NodeTemplate->GenericGraphNode->SetFlags(RF_Transactional);
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

EGraphType UStateMachineSchema::GetGraphType(const UEdGraph* TestEdGraph) const
{
	return GT_StateMachine;
}

// Helper function to format the right-click context menu of the graph editor.
FText GetFormattedMenuText(TSubclassOf<UStateNode> NodeType) 
{
	FString ClassString = NodeType->GetMetaData("Category");

	if (ClassString.StartsWith("StateMachine|")) {
		ClassString.RightChopInline(13);
	}
	else if (ClassString.StartsWith("StateMachine")) {
		ClassString.RightChopInline(12);
	}

	return FText::Format(LOCTEXT("StateMAchineGraphNodeAction", "{0}"), FText::FromString(ClassString));
}

void UStateMachineSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const bool bNoParent = (ContextMenuBuilder.FromPin == NULL);

	const FText AddToolTip = LOCTEXT("NewStateGraphNodeTooltip", "Add node here");

	TSet<TSubclassOf<UStateNode> > Visited;

	for (TObjectIterator<UClass> It; It; ++It)
	{
		bool c1 = (*It)->IsChildOf(UStateNode::StaticClass());
		bool c2 = !It->HasAnyClassFlags(CLASS_Abstract);
		bool c3 = !Visited.Contains(*It);
		bool c4 = !(*It)->HasAnyClassFlags(CLASS_Abstract);

		if ( c1 && c2 && c3 && c4)
		{
			TSubclassOf<UStateNode> NodeType = *It;

			if (It->GetName().StartsWith("REINST") || It->GetName().StartsWith("SKEL"))
				continue;

			//if (!Graph->GetClass()->IsChildOf(NodeType.GetDefaultObject()->CompatibleGraphType))
			//	continue;

			//FText Desc = FText::FromString(NodeType->GetDefaultObjectName().ToString());
			FString Title = NodeType->GetName();
			Title.RemoveFromEnd("_C");
			FText Desc = FText::FromString(Title);
			
			FText MenuText = GetFormattedMenuText(NodeType);
			
			TSharedPtr<FSMSchemaAction_NewNode> Action(
				new FSMSchemaAction_NewNode(MenuText, Desc, AddToolTip, 0));

			Action->NodeTemplate = NewObject<UEdStateNode>(ContextMenuBuilder.OwnerOfTemporaries);

			//Action->NodeTemplate->GenericGraphNode->Graph = Graph;
			ContextMenuBuilder.AddAction(Action);

			Visited.Add(NodeType);
		}
	}
}



#pragma endregion

#undef LOCTEXT_NAMESPACE