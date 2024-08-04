#include "StateMachine/StateMachineBlueprintGeneratedClass.h"

FName GenerateStateName(UStateMachine* Outer, FName StateName)
{
	FString NameBase = "";
	NameBase.Append(Outer->GetName());
	NameBase.Append("::");
	NameBase.Append(StateName.ToString());

	return FName(NameBase);
}

bool UStateMachineBlueprintGeneratedClass::GetStateData(
	FStateData& Output,
	UStateMachine* Outer,
	FName MachineName,
	FName StateName)
{
	UStateMachineArchetype* Machine = nullptr;

	if (MachineName == NAME_None)
	{
		Machine = this->StateMachineArchetype;
	}
	else if (this->SubStateMachineArchetypes.Contains(MachineName)) 
	{
		Machine = this->SubStateMachineArchetypes[MachineName];
	}

	if (Machine)
	{
		auto ArchData = Machine->GetStateData(StateName);

		if (ArchData)
		{
			Output.Transitions.Append(ArchData->Transitions);
			Output.Node = DuplicateObject(ArchData->Node, Outer, GenerateStateName(Outer, StateName));
			return true;
		}		
	}

	return false;
}