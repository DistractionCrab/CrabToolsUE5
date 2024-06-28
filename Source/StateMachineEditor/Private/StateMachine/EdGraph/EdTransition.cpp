#include "StateMachine/EdGraph/EdTransition.h"
#include "StateMachine/EdGraph/EdBaseNode.h"
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

UEdBaseStateNode* UEdTransition::GetStartNode()
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

UEdBaseStateNode* UEdTransition::GetEndNode()
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


/*
TArray<FString> UEdTransition::GetConditionOptions() const
{
	TArray<FString> Names;
	auto GraphRef = Cast<UEdStateGraph>(this->GetGraph());
	auto BP = GraphRef->GetBlueprintOwner();	
	auto base = BP->GeneratedClass->FindFunctionByName("TrueCondition");

	if (base)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found the base function?"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Did not find the base function?!?"));
	}

	for (TFieldIterator<UFunction> FIT(BP->GeneratedClass, EFieldIteratorFlags::IncludeSuper); FIT; ++FIT)
	{
		UFunction* f = *FIT;

		UE_LOG(LogTemp, Warning, TEXT("Found function: %s"), *f->GetFName().ToString());

		if (f->IsSignatureCompatibleWith(base))
		{
			Names.Add(f->GetName());
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}
*/

#undef LOCTEXT_NAMESPACE

