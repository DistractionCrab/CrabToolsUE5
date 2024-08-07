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
	TSubclassOf<UStateMachine> ArchetypeClass;
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

	bool GetStateData(FStateData& Output, UStateMachine* Outer, FName Machine, FName StateName);
};