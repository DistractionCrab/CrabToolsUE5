#include "StateMachine/ProcStateMachineBlueprint.h"

UProcStateMachineBlueprint::UProcStateMachineBlueprint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UClass* UProcStateMachineBlueprint::GetBlueprintClass() const {
	return UProcStateMachineBlueprintGeneratedClass::StaticClass();
}

bool UProcStateMachineBlueprint::SupportsInputEvents() const  {
	return true;
}