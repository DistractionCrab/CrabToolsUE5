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

UState* UStateMachineBlueprintGeneratedClass::GetStateData(
	UStateMachine* Outer,
	FName MachineName,
	FName StateName)
{
	UStateMachineArchetype* Machine = nullptr;
	UState* BuiltState = nullptr;

	if (StateName.IsNone())
	{
		return nullptr;
	}

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
		auto DefaultState = Machine->GetStateData(StateName);

		if (DefaultState)
		{
			BuiltState = DuplicateObject(DefaultState, Outer);
			// Need to append the state data, as those are not duplicated.
			BuiltState->Append(DefaultState);

			if (!BuiltState->GetOverride())
			{
				if (auto ParentClass = Cast<UStateMachineBlueprintGeneratedClass>(this->GetSuperClass()))
				{
					BuiltState->Append(ParentClass->GetStateData(Outer, NAME_None, StateName));
				}
			}
		}
	}	

	return BuiltState;
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

	for (auto& SubMachine : this->SubStateMachineArchetypes)
	{
		UE_LOG(LogTemp, Warning, TEXT("- Found SubMachines: %s"), *SubMachine.Key.ToString());
		bool Check = SubMachine.Value->Accessibility == EStateMachineAccessibility::PUBLIC
			|| SubMachine.Value->Accessibility == EStateMachineAccessibility::PROTECTED
			|| SubMachine.Value->Accessibility == EStateMachineAccessibility::OVERRIDEABLE;

		if (Check)
		{
			UE_LOG(LogTemp, Warning, TEXT("- Added SubMachines: %s"), *SubMachine.Key.ToString());
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

TArray<FName> UStateMachineBlueprintGeneratedClass::GetSubMachineOptions() const
{
	TArray<FName> Names;

	for (auto& SubMachine : this->SubStateMachineArchetypes)
	{
		Names.Add(SubMachine.Key);
	}

	return Names;
}

UStateMachine* UStateMachineBlueprintGeneratedClass::ConstructSubMachine(UStateMachine* Outer, FName Key) const
{
	UStateMachine* Constructed = nullptr;

	if (auto Ptr = this->SubStateMachineArchetypes.Find(Key))
	{
		if (auto SM = Ptr->Get())
		{
			Constructed = SM->CreateStateMachine(Outer, Key);

			if (SM->bIsVariable)
			{
				FProperty* Prop = this->FindPropertyByName(Key);

				if (Prop && Prop->GetClass() == FObjectProperty::StaticClass())
				{
					FObjectProperty* ObjProp = (FObjectProperty*)Prop;

					if (ObjProp->PropertyClass == Constructed->GetClass())
					{
						ObjProp->SetValue_InContainer(Outer, Constructed);
					}
				}
			}
		}
	}

	return Constructed;
}

EStateMachineAccessibility UStateMachineBlueprintGeneratedClass::GetSubMachineAccessibility(FName Key) const
{
	if (auto SMPtr = this->SubStateMachineArchetypes.Find(Key))
	{
		if (auto SM = SMPtr->Get())
		{
			return SM->Accessibility;
		}
		else
		{
			return EStateMachineAccessibility::PRIVATE;
		}
	}
	else if (auto Parent = Cast<UStateMachineBlueprintGeneratedClass>(this->GetSuperClass()))
	{
		return Parent->GetSubMachineAccessibility(Key);
	}
	else
	{
		return EStateMachineAccessibility::PRIVATE;
	}
}

void UStateMachineBlueprintGeneratedClass::CollectExtendibleStates(
	TSet<FString>& StateNames,
	FName SubMachineName) const
{
	if (SubMachineName.IsNone())
	{
		this->StateMachineArchetype->CollectExtendibleStates(StateNames);
	}
	else
	{
		if (auto SubM = this->SubStateMachineArchetypes.Find(SubMachineName))
		{
			SubM->Get()->CollectExtendibleStates(StateNames);
		}
	}
}

UStateMachine* UStateMachineBlueprintGeneratedClass::DuplicateSubMachineArchetype(FName SubMachineName, UObject* Outer) const
{
	if (auto Machine = this->SubStateMachineArchetypes.Find(SubMachineName))
	{
		return DuplicateObject(Machine->Get()->ArchetypeObject, Outer);
	}

	return nullptr;
}