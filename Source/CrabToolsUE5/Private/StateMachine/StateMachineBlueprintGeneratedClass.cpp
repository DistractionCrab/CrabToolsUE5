#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/Utils.h"

UStateMachine* UStateMachineArchetype::CreateStateMachine(UStateMachine* Parent, FName ParentKey)
{
	auto NewMachine = NewObject<UStateMachine>(Parent, this->ArchetypeClass.Get(), ParentKey);
	NewMachine->ParentMachine = this;
	NewMachine->ParentKey = Key;
	NewMachine->StartState = this->StartState;

	return NewMachine;
}

bool UStateMachineBlueprintGeneratedClass::GetStateData(
	FStateData& Output,
	UStateMachine* Outer,
	FName MachineName,
	FName StateName)
{
	UStateMachineArchetype* Machine = nullptr;
	bool FoundData = false;

	if (MachineName == NAME_None)
	{
		Machine = this->StateMachineArchetype;

		if (auto ParentClass = Cast<UStateMachineBlueprintGeneratedClass>(this->GetSuperClass()))
		{
			FStateData Compile;
			FoundData = ParentClass->GetStateData(Compile, Outer, NAME_None, StateName);

			if (FoundData)
			{
				Output.Append(Compile, Outer);
			}
		}
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
			Output.AppendCopy(*ArchData, Outer);
		}		
	}	

	return FoundData;
}

FName UStateMachineBlueprintGeneratedClass::GetStartState() const
{
	// Case for when default objects are constructed, but class isn't prepped.
	if (!this->StateMachineArchetype) { return NAME_None; }

	if (this->StateMachineArchetype->StartState == NAME_None)
	{
		if (auto Parent = Cast<UStateMachineBlueprintGeneratedClass>(this->GetSuperClass()))
		{
			return Parent->GetStartState();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Could not find start state for SM-class: %s"),
				*this->GetName());
			return NAME_None;
		}
	}
	else
	{
		return this->StateMachineArchetype->StartState;
	}
}