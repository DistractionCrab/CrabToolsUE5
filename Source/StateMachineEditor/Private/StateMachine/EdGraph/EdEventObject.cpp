#include "StateMachine/EdGraph/EdEventObject.h"
#include "STateMachine/EdGraph/EdStateGraph.h"
#include "StateMachine/StateMachineBlueprint.h"

#define LOCTEXT_NAMESPACE "UEdEventObject"

void UEdEventObject::Inspect()
{
	if (auto Graph = this->GetGraph())
	{
		Graph->GetBlueprintOwner()->Events.OnObjectInspected.Broadcast(this);
	}
}

FName UEdEventObject::RenameEvent(FName NewName)
{
	return this->GetGraph()->RenameEvent(this, NewName);
}

void UEdEventObject::SetName(FName Name)
{
	const FScopedTransaction Transaction(LOCTEXT("SetEventName", "Set Event Name"));
	this->Modify();

	FName OldName = this->EventName;
	this->EventName = Name;
	this->Events.OnEventRenamed.Broadcast(OldName, Name);
}

UEdStateGraph* UEdEventObject::GetGraph() const
{
	auto Graph = Cast<UEdStateGraph>(GetOuter());

	if (Graph == nullptr && IsValid(this))
	{
		if (GetOuter() != GetTransientPackage())
		{
			ensureMsgf(false, TEXT("EdEventObject::GetGraph : '%s' does not have a UEdStateGraph as an Outer."), *GetPathName());
		}
	}
	return Graph;
}

void UEdEventObject::Delete()
{
	this->GetGraph()->RemoveEvent(this);
}

bool UEdEventObject::Modify(bool bAlwaysMarkDirty)
{
	Super::Modify(bAlwaysMarkDirty);
	return this->GetGraph()->Modify(bAlwaysMarkDirty);
}

#undef LOCTEXT_NAMESPACE