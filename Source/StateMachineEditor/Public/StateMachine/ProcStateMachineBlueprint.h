#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "StateMachine/ProcStateMachineBlueprintGeneratedClass.h"

#include "ProcStateMachineBlueprint.generated.h"

UCLASS(BlueprintType)
class STATEMACHINEEDITOR_API UProcStateMachineBlueprint : public UBlueprint
{
	GENERATED_UCLASS_BODY()

public:
	UClass* GetBlueprintClass() const override;
	virtual bool SupportsInputEvents() const override;
};