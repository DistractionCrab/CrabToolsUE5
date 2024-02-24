// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachine/HierarchyNode.h"

void UHierarchyNode::Initialize_Implementation(UProcStateMachine* POwner) {
	UStateNode::Initialize(POwner);

	if (this->MachineClass.Get() != nullptr) {
		this->SubMachine = NewObject<UProcStateMachine>(
			this,
			this->MachineClass, 
			NAME_None, 
			RF_NoFlags,
			this->MachineClass.GetDefaultObject(), 
			true);

		for (auto& Pair : this->SubstituteNodes) {
			this->SubMachine->Substitute(Pair.Key, Pair.Value);
		}

		this->SubMachine->Initialize_Internal(POwner->GetOwner());
	}
}

void UHierarchyNode::PerformExit() {

	if (this->SubMachine != nullptr) {
		FName SubStateName = this->SubMachine->GetCurrentStateName();

		if (this->ExitStates.Contains(SubStateName)) {
			this->GoTo(this->ExitStates[SubStateName]);
		}		
	}
}

void UHierarchyNode::Event_Implementation(FName EName){
	if (this->SubMachine) {
		this->SubMachine->Event(EName);
		this->PerformExit();
	}
}

void UHierarchyNode::EventWithData_Implementation(FName EName, UObject* Data) {
	if (this->SubMachine) {
		this->SubMachine->EventWithData(EName, Data);
		this->PerformExit();
	}
}

void UHierarchyNode::Enter_Implementation() {
	if (this->SubMachine) {
		if (this->ResetOnEnter) {
			this->SubMachine->Reset();
		}
		this->SubMachine->Event(this->EnterEventName);
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

UStateNode* UHierarchyNode::Substitute(FName SlotName, UStateNode* Node) {
	for (auto& Pair : this->SubstituteNodes) {
		this->SubstituteNodes[Pair.Key] = Pair.Value->Substitute(SlotName, Node);
	}

	return this;
}