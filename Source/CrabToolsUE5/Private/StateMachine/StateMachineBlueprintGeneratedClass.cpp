#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "StateMachine/Utils.h"

UStateMachine* UStateMachineArchetype::CreateStateMachine(UStateMachine* Parent, FName NewParentKey)
{
	check(this->ArchetypeObject)

	auto NewMachine = DuplicateObject<UStateMachine>(this->ArchetypeObject, Parent, NewParentKey);
	NewMachine->SetParentData(Parent, NewParentKey);
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
			FoundData = true;
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

TArray<FString> UStateMachineBlueprintGeneratedClass::GetChildAccessibleSubMachines() const
{
	TArray<FString> Names;

	for (auto SubMachine : this->SubStateMachineArchetypes)
	{
		bool Check = SubMachine.Value->Accessibility == EStateMachineAccessibility::PUBLIC
			|| SubMachine.Value->Accessibility == EStateMachineAccessibility::PROTECTED
			|| SubMachine.Value->Accessibility == EStateMachineAccessibility::OVERRIDEABLE;

		if (Check)
		{
			Names.Add(SubMachine.Key.ToString());
		}
	}

	if (auto BPGC = Cast<UStateMachineBlueprintGeneratedClass>(this->GetSuperClass()))
	{
		Names.Append(BPGC->GetChildAccessibleSubMachines());
	}

	return Names;
}

TArray<FString> UStateMachineBlueprintGeneratedClass::GetStateOptions(EStateMachineAccessibility Access) const
{
	TArray<FString> Names;

	Names.Append(this->StateMachineArchetype->GetStatesWithAccessibility(Access));

	return Names;
}