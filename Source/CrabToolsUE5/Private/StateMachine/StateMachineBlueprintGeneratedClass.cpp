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

		if (DefaultState->GetAccess() == EStateMachineAccessibility::PRIVATE)
		{
			BuiltState = DuplicateObject(DefaultState, Outer);
		}
		else if (DefaultState->GetAccess() == EStateMachineAccessibility::PROTECTED)
		{

		}

		if (DefaultState)
		{
			BuiltState = DuplicateObject(DefaultState, Outer);
			// Need to append the state data, as those are not duplicated.
			BuiltState->Append(DefaultState);

			if (BuiltState->GetAccess() != EStateMachineAccessibility::OVERRIDEABLE)
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
	else if (auto Parent = this->GetParent())
	{
		return Parent->DuplicateSubMachineArchetype(SubMachineName, Outer);
	}

	return nullptr;
}

UState* UStateMachineBlueprintGeneratedClass::DuplicateStateArchetype(FName MachineName, FName StateName, UObject* Outer) const
{
	/* The machine to check for in this class level. */
	UStateMachineArchetype* Machine = nullptr;
	UState* CopiedState = nullptr;

	if (MachineName.IsNone())
	{
		Machine = this->StateMachineArchetype;
	}
	else
	{
		Machine = this->SubStateMachineArchetypes.Find(MachineName)->Get();
	}	

	if (Machine == nullptr)
	{
		if (auto Parent = this->GetParent())
		{
			CopiedState = Parent->DuplicateStateArchetype(MachineName, StateName, Outer);
		}
	}
	else
	{
		UState* Check = Machine->GetStateData(StateName);

		if (Check)
		{
			if (Check->GetAccess() == EStateMachineAccessibility::OVERRIDEABLE)
			{
				CopiedState = DuplicateObject(Check, Outer);
			}
			else if (Check->GetAccess() == EStateMachineAccessibility::PROTECTED)
			{
				CopiedState = this->GetParent()->DuplicateStateArchetype(MachineName, StateName, Outer);
			}
			else if (Check->GetAccess() == EStateMachineAccessibility::PUBLIC)
			{
				CopiedState = DuplicateObject(Check, Outer);
			}
			else if (Check->GetAccess() == EStateMachineAccessibility::OVERRIDE_EXTEND)
			{
				CopiedState = DuplicateObject(Check, Outer);
			}
			else if (Check->GetAccess() == EStateMachineAccessibility::PRIVATE)
			{
				CopiedState = DuplicateObject(Check, Outer);
			}
			else
			{
				UE_LOG(LogStateMachine, Error, TEXT("Invalid inheritance access for state: %s found %s"),
					*StateName.ToString(),
					*UEnum::GetValueAsString(Check->GetAccess()));
			}
		}
		else if (auto Parent = this->GetParent())
		{
			CopiedState = Parent->DuplicateStateArchetype(MachineName, StateName, Outer);
		}
	}

	return CopiedState;
}

const UState* UStateMachineBlueprintGeneratedClass::GetStateArchetype(FName MachineName, FName StateName) const
{
	/* The machine to check for in this class level. */
	UStateMachineArchetype* Machine = nullptr;
	const UState* CopiedState = nullptr;

	if (MachineName.IsNone())
	{
		Machine = this->StateMachineArchetype;
	}
	else
	{
		Machine = this->SubStateMachineArchetypes.Find(MachineName)->Get();
	}

	if (Machine == nullptr)
	{
		if (auto Parent = this->GetParent())
		{
			CopiedState = Parent->GetStateArchetype(MachineName, StateName);
		}
	}
	else
	{
		const UState* Check = Machine->GetStateData(StateName);

		if (Check)
		{
			if (Check->GetAccess() == EStateMachineAccessibility::OVERRIDEABLE)
			{
				CopiedState = Check;
			}
			else if (Check->GetAccess() == EStateMachineAccessibility::PROTECTED)
			{
				CopiedState = this->GetParent()->GetStateArchetype(MachineName, StateName);
			}
			else if (Check->GetAccess() == EStateMachineAccessibility::PUBLIC)
			{
				CopiedState = Check;
			}
			else if (Check->GetAccess() == EStateMachineAccessibility::OVERRIDE_EXTEND)
			{
				CopiedState = Check;
			}
			else if (Check->GetAccess() == EStateMachineAccessibility::PRIVATE)
			{
				CopiedState = Check;
			}
			else
			{
				UE_LOG(LogStateMachine, Error, TEXT("Invalid inheritance access for state: %s found %s"),
					*StateName.ToString(),
					*UEnum::GetValueAsString(Check->GetAccess()));
			}
		}
		else if (auto Parent = this->GetParent())
		{
			CopiedState = Parent->GetStateArchetype(MachineName, StateName);
		}
	}

	return CopiedState;
}

const UStateMachine* UStateMachineBlueprintGeneratedClass::GetSubMachineArchetype(FName SubMachineName) const
{
	if (auto Machine = this->SubStateMachineArchetypes.Find(SubMachineName))
	{
		return Machine->Get()->ArchetypeObject;
	}
	else if (auto Parent = this->GetParent())
	{
		return Parent->GetSubMachineArchetype(SubMachineName);
	}

	return nullptr;
}

const UStateMachineArchetype* UStateMachineBlueprintGeneratedClass::GetSubMachineArchetypeData(FName SubMachineName) const
{
	if (SubMachineName.IsNone())
	{
		return this->StateMachineArchetype;
	}
	else
	{
		if (auto Machine = this->SubStateMachineArchetypes.Find(SubMachineName))
		{
			return Machine->Get();
		}
	}

	return nullptr;
}

bool UStateMachineBlueprintGeneratedClass::IsSubMachineNameInUse(FString& Name) const
{
	for (auto& SubGraph : this->SubStateMachineArchetypes)
	{
		if (SubGraph.Key == Name)
		{
			return true;
		}
	}

	if (auto Parent = this->GetParent())
	{
		return Parent->IsSubMachineNameInUse(Name);
	}
	else
	{
		return false;
	}
}

UStateMachine* UStateMachineBlueprintGeneratedClass::GetMostRecentParentArchetype(FName SubMachineKey) const
{
	if (this->SubStateMachineArchetypes.Contains(SubMachineKey))
	{
		return this->SubStateMachineArchetypes[SubMachineKey]->ArchetypeObject;
	}
	else
	{
		if (auto Parent = this->GetParent())
		{
			return Parent->GetMostRecentParentArchetype(SubMachineKey);
		}
		else
		{
			return nullptr;
		}
	}
}