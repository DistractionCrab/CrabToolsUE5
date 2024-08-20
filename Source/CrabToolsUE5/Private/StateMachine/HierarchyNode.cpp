#include "StateMachine/HierarchyNode.h"

#include "StateMachine/IStateMachineLike.h"

void UHierarchyNode::Initialize_Implementation()
{
	UStateNode::Initialize_Implementation();
	UE_LOG(LogTemp, Warning, TEXT("HierarchyNode::Init: %s"), *this->SlotName.ToString());

	this->SubMachine = this->GetMachine()->GetSubMachine(this->SlotName);
	
	if (this->SubMachine)
	{
		UE_LOG(LogTemp, Warning, TEXT("HierarchyNode::Init: Found SubMachine"));
		this->SubMachine->Initialize_Internal(this->GetMachine()->GetOwner());
	}
}

void UHierarchyNode::PerformExit()
{
	if (this->SubMachine != nullptr) {
		FName SubStateName = this->SubMachine->GetCurrentStateName();

		if (this->ExitStates.Contains(SubStateName))
		{
			this->GetMachine()->Event(this->ExitStates[SubStateName]);
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

// Editor helper functions.
#if WITH_EDITORONLY_DATA
	TArray<FString> UHierarchyNode::GetMachineOptions() const
	{
		const UObject* Outer = this;

		do
		{
			if (auto SMLike = Cast<IStateMachineLike>(Outer))
			{
				return SMLike->GetMachineOptions();
			}
			Outer = Outer->GetOuter();

		} while (Outer);

		return { };
	}
#endif