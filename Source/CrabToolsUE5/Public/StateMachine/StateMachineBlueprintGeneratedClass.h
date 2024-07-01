#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Binding/DynamicPropertyPath.h"
#include "Engine/BlueprintGeneratedClass.h"

#include "StateMachineBlueprintGeneratedClass.generated.h"

struct FStateData;
class UStateMachine;

UCLASS(MinimalAPI)
class UStateMachineBlueprintGeneratedClass : public UBlueprintGeneratedClass
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UStateMachine> StateMachineArchetype;

	FStateData GetStateData(FName StateName);
};