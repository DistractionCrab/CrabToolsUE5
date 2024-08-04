#include "StateMachine/EdGraph/EdTransition.h"
#include "StateMachine/EdGraph/EdBaseNode.h"
#include "StateMachine/EdGraph/EdStateNode.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/EdGraph/EdEventObject.h"
#include "StateMachine/StateMachineBlueprint.h"


#define LOCTEXT_NAMESPACE "EdNode_GenericGraphEdge"

UEdTransition::UEdTransition()
{
	bCanRenameNode = true;
}


void UEdTransition::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, TEXT("Edge"), FName(), TEXT("In"));
	Inputs->bHidden = true;
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, TEXT("Edge"), FName(), TEXT("Out"));
	Outputs->bHidden = true;
}

FText UEdTransition::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText();
}

void UEdTransition::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (Pin->LinkedTo.Num() == 0)
	{
		// Commit suicide; transitions must always have an input and output connection
		Modify();

		// Our parent graph will have our graph in SubGraphs so needs to be modified to record that.
		if (UEdGraph* ParentGraph = GetGraph())
		{
			ParentGraph->Modify();
		}

		DestroyNode();
	}
}

void UEdTransition::PrepareForCopying()
{
	
}

void UEdTransition::CreateConnections(UEdBaseStateNode* Start, UEdBaseStateNode* End)
{
	Pins[0]->Modify();
	Pins[0]->LinkedTo.Empty();

	Start->GetOutputPin()->Modify();
	Pins[0]->MakeLinkTo(Start->GetOutputPin());

	// This to next
	Pins[1]->Modify();
	Pins[1]->LinkedTo.Empty();

	End->GetInputPin()->Modify();
	Pins[1]->MakeLinkTo(End->GetInputPin());
}

UEdBaseStateNode* UEdTransition::GetStartNode() const
{
	if (Pins[0]->LinkedTo.Num() > 0)
	{
		return Cast<UEdBaseStateNode>(Pins[0]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return nullptr;
	}
}

UEdBaseStateNode* UEdTransition::GetEndNode() const
{
	if (Pins[1]->LinkedTo.Num() > 0)
	{
		return Cast<UEdBaseStateNode>(Pins[1]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return nullptr;
	}
}



TArray<FString> UEdTransition::GetEventOptions() const
{
	if (auto StateNode = Cast<UEdStateNode>(this->GetStartNode()))
	{
		return StateNode->GetEventOptions();
	}

	return Cast<UEdStateGraph>(this->GetGraph())->GetEventOptions();
}

TArray<FString> UEdTransition::GetConditionOptions() const
{
	return Cast<UEdStateGraph>(this->GetGraph())->GetConditionOptions();
}

TMap<FName, FTransitionData> UEdTransition::GetTransitionData(FKismetCompilerContext& Context)
{
	TMap<FName, FTransitionData> Data;

	for (auto Values : this->EventToConditionMap)
	{
		FTransitionData DataValue
		{
			this->GetEndNode()->GetStateName(),
			Values.Value.Condition,
			Values.Value.DataCondition,
		};

		if (!this->GetStateGraph()->HasEvent(Values.Key))
		{

		}

		Data.Add(Values.Key, DataValue);
	}

	return Data;
}

#undef LOCTEXT_NAMESPACE

