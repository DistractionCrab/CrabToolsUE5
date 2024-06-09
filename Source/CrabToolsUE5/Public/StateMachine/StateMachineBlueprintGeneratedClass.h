#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Binding/DynamicPropertyPath.h"
#include "Engine/BlueprintGeneratedClass.h"

#include "StateMachineBlueprintGeneratedClass.generated.h"

class UStateMachine;

UCLASS(MinimalAPI)
class UStateMachineBlueprintGeneratedClass : public UBlueprintGeneratedClass
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TObjectPtr<UStateMachine> StateMachineArchetype;

};