#include "StateMachine/EdGraph/EdAliasNode.h"
#include "StateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/EdGraph/EdTransition.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "StateMachine/StateMachineBlueprintGeneratedClass.h"

#define LOCTEXT_NAMESPACE "EdAliasNode"

UEdAliasNode::UEdAliasNode() {
	this->bCanRenameNode = true;
}

UEdAliasNode::~UEdAliasNode() {

}

FName UEdAliasNode::GetStateName() const
{ 
	return this->AliasLabel;
}

FName UEdAliasNode::SetStateName(FName NewName)
{
	FName OldName = this->AliasLabel;
	this->AliasLabel = NewName;
	this->Events.OnNameChanged.Broadcast(OldName, NewName);

	return NewName;
}

void UEdAliasNode::RenameNode(FName Name)
{
	this->GetStateGraph()->RenameNode(this, Name);
}

bool UEdAliasNode::Matches(UEdStateNode* Node) const
{
	return this->AliasedStates.Contains(Node->GetStateName()) != this->bComplement;
}

void UEdAliasNode::Delete()
{
	this->Modify();

	const UEdGraphSchema* Schema = this->GetSchema();
	if (Schema != nullptr)
	{
		Schema->BreakNodeLinks(*this);
	}

	this->DestroyNode();
}

bool UEdAliasNode::Modify(bool bAlwaysMarkDirty)
{
	Super::Modify(bAlwaysMarkDirty);
	return this->GetGraph()->Modify(bAlwaysMarkDirty);
}

bool UEdAliasNode::HasEvent(FName EName)
{
	for (auto State : this->GetStateGraph()->GetStates())
	{
		if (this->AliasedStates.Contains(State->GetStateName()) != this->bComplement)
		{
			if (State->HasEvent(EName))
			{
				return true;
			}
		}
	}

	return this->GetStateGraph()->HasEvent(EName);;
}


#if WITH_EDITOR

void UEdAliasNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);


}

#endif

#undef LOCTEXT_NAMESPACE