#include "StateMachine/HierarchyNode.h"
#include "StateMachine/IStateMachineLike.h"
#include "Utils/UtilsLibrary.h"

void UHierarchyNode::Initialize_Implementation()
{
	UStateNode::Initialize_Implementation();

	FString Address = this->SlotName.ToString();
	
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
	if (this->SubMachine) {
		this->SubMachine->Event_Direct(EName);
		this->PerformExit();
	}
}

void UHierarchyNode::EventWithData_Implementation(FName EName, UObject* Data) {
	if (this->SubMachine) {
		this->SubMachine->EventWithData_Direct(EName, Data);
		this->PerformExit();
	}
}

void UHierarchyNode::Enter_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("Entering HierarchyNode"));
	if (this->SubMachine) {
		if (this->ResetOnEnter) {
			this->SubMachine->Reset();
		}
		this->SubMachine->Event_Direct(this->EnterEventName);
	}
}

void UHierarchyNode::Tick_Implementation(float DeltaTime) {
	if (this->SubMachine) {
		this->SubMachine->Tick(DeltaTime);
		this->PerformExit();
	}
}

void UHierarchyNode::Exit_Implementation() {

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
TArray<FString> UHierarchyNode::GetMachineOptions() const
{
	const UObject* Outer = this;

	if (auto SMLike = UtilsFunctions::GetOuterAs<IStateMachineLike>(Outer))
	{
		return SMLike->GetMachineOptions();
	}

	return { };
}

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
		this->SlotName = NAME_None;
	}
}

TArray<FString> UHierarchyNode::GetSubMachineStateOptions() const
{
	TArray<FString> Names;

	if (IsValid(this->SubMachine))
	{
		UE_LOG(LogTemp, Warning, TEXT("Getting SubMachine Options?"));
		Names.Append(this->SubMachine->GetStateOptions(this));
	}
	else
	{
		if (auto Outer = UtilsFunctions::GetOuterAs<IStateMachineLike>(this))
		{
			Names.Append(Outer->GetStateOptions(this));
		}
	}

	Names.Sort([&](const FString& A, const FString& B) { return A < B; });

	return Names;
}

#endif