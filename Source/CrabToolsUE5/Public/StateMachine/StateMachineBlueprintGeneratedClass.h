#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Binding/DynamicPropertyPath.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "StateMachine/StateMachine.h"
#include "StateMachineBlueprintGeneratedClass.generated.h"

struct FStateData;
class UStateMachine;

UCLASS(NotBlueprintable, HideDropdown, Hidden)
class CRABTOOLSUE5_API UStateMachineArchetype : public UStateMachine
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool bInstanceEditable = false;

	UPROPERTY()
	bool bIsVariable = false;

	UPROPERTY()
	TObjectPtr<UStateMachine> ArchetypeObject;

	UPROPERTY()
	EStateMachineAccessibility Accessibility = EStateMachineAccessibility::PRIVATE;

	UStateMachine* CreateStateMachine(UStateMachine* Parent, FName ParentKey);
};



UCLASS()
class CRABTOOLSUE5_API UStateMachineBlueprintGeneratedClass : public UBlueprintGeneratedClass
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TObjectPtr<UStateMachineArchetype> StateMachineArchetype;

	UPROPERTY()
	TMap<FName, TObjectPtr<UStateMachineArchetype>> SubStateMachineArchetypes;

	UPROPERTY()
	TSet<FName> EventSet;

	UPROPERTY();
	TSet<FName> EmittedEvents;

public:

	UState* GetStateData(UStateMachine* Outer, FName Machine, FName StateName);
	FName GetStartState() const;
	TArray<FString> GetStateOptions(EStateMachineAccessibility Access = EStateMachineAccessibility::PUBLIC) const;
	TArray<FString> GetChildAccessibleSubMachines() const;
	EStateMachineAccessibility GetSubMachineAccessibility(FName Key) const;
	TArray<FName> GetSubMachineOptions() const;
	UStateMachine* ConstructSubMachine(UStateMachine* Outer, FName Key) const;

	void CollectExtendibleStates(TSet<FString>& StateNames, FName SubMachineName = NAME_None) const;

	UStateMachine* DuplicateSubMachineArchetype(FName SubMachineName, UObject* Outer) const;

	UStateMachineBlueprintGeneratedClass* GetParent() const
	{
		return Cast<UStateMachineBlueprintGeneratedClass>(this->GetSuperClass());
	}

	bool IsSubMachineNameInUse(FString& Name) const;

	UStateMachine* GetMostRecentParentArchetype(FName SubMachineKey) const;
};