#include "StateMachine/HierarchyNode.h"
#include "StateMachine/IStateMachineLike.h"
#include "Utils/UtilsLibrary.h"

void UHierarchyNode::Initialize_Implementation()
{
	UStateNode::Initialize_Implementation();

	FString Address = this->Slot.MachineName.ToString();
	
	if (auto Machine = Cast<UStateMachine>(this->GetMachine()->GetSubMachine(Address)))
	{
		this->SubMachine = Machine;
		this->SubMachine->Initialize_Internal(this->GetMachine()->GetOwner());
	}
}

void UHierarchyNode::PerformExit()
{
	if (this->SubMachine != nullptr) {
		FName SubStateName = this->SubMachine->GetCurrentStateName();

		if (this->ExitStates.Contains(SubStateName))
		{
			this->GetMachine()->Event(this->ExitStates[SubStateName].GetEvent());
		}
	}
}

void UHierarchyNode::Event_Implementation(FName EName){
	if (this->SubMachine)
	{
		this->SubMachine->SendEvent(EName);
	}
}

void UHierarchyNode::EventWithData_Implementation(FName EName, UObject* Data)
{
	if (this->SubMachine)
	{
		this->SubMachine->SendEventWithData(EName, Data);
	}
}

void UHierarchyNode::PostTransition_Implementation()
{
	this->PerformExit();
}

void UHierarchyNode::Enter_Implementation() {
	if (this->SubMachine) {
		this->SubMachine->SetActive(true);
		if (this->ResetOnEnter)
		{
			this->SubMachine->Reset();
		}
		else
		{
			this->SubMachine->SendEvent(this->EnterEventName);
		}
	}
}

void UHierarchyNode::Tick_Implementation(float DeltaTime) {
	if (this->SubMachine) {
		this->SubMachine->Tick(DeltaTime);
		this->PerformExit();
	}
}

void UHierarchyNode::Exit_Implementation()
{
	if (this->SubMachine)
	{
		this->SubMachine->SendEvent(this->ExitEventName);
		this->SubMachine->SetActive(false);
	}
}

FName FHierarchyEventValue::GetEvent() const
{
	if (this->EventType == EHierarchyInputType::INLINED)
	{
		return this->InlinedEvent;
	}
	else
	{
		return this->DefinedEvent;
	}
}



// Editor helper functions.
#if WITH_EDITOR
void UHierarchyNode::GetEmittedEvents(TSet<FName>& Events) const
{
	Super::GetEmittedEvents(Events);

	if (auto Outer = UtilsFunctions::GetOuterAs<IStateMachineLike>(this))
	{
		Events.Append(Outer->GetEmittedEvents());
	}
}

void UHierarchyNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (this->StateMachineSource == EHierarchyInputType::DEFINED)
	{
		this->SubMachine = nullptr;
	}
	else if (this->StateMachineSource == EHierarchyInputType::INLINED)
	{
		this->Slot.MachineName = NAME_None;
	}
}

TArray<FString> UHierarchyNode::GetSubMachineStateOptions() const
{
	TArray<FString> Names;

	if (IsValid(this->SubMachine))
	{
		Names.Append(this->SubMachine->GetStateOptions(this));
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

TArray<FString> UHierarchyNode::GetSubMachineTransitionEvents() const
{
	TArray<FString> Names;

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}


TArray<FString> UHierarchyNode::GetStateEventOptions() const
{
	TArray<FString> Names;

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

#endif