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
	TObjectPtr<UStateMachine> ArchetypeObject;

	UPROPERTY()
	EStateMachineAccessibility Accessibility = EStateMachineAccessibility::PRIVATE;

	UStateMachine* CreateStateMachine(UStateMachine* Parent, FName ParentKey);
};



UCLASS(MinimalAPI)
class UStateMachineBlueprintGeneratedClass : public UBlueprintGeneratedClass
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

	bool GetStateData(FStateData& Output, UStateMachine* Outer, FName Machine, FName StateName);
	FName GetStartState() const;

	TArray<FString> GetStateOptions(EStateMachineAccessibility Access = EStateMachineAccessibility::PUBLIC) const;

	CRABTOOLSUE5_API TArray<FString> GetChildAccessibleSubMachines() const;
};