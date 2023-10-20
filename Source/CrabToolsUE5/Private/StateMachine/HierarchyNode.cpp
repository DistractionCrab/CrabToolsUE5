// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachine/HierarchyNode.h"

void UHierarchyNode::Initialize(UProcStateMachine* POwner) {
	UStateNode::Initialize(POwner);

	if (this->MachineClass.Get() != nullptr) {
		this->Machine = NewObject<UProcStateMachine>(
			this, 
			this->MachineClass, 
			NAME_None, 
			RF_NoFlags,
			this->MachineClass.GetDefaultObject(), 
			true);
	}
}