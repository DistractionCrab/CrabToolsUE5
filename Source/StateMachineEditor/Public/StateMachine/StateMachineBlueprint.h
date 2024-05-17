#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "StateMachine/EdGraph/EdStateGraph.h"

#include "StateMachineBlueprint.generated.h"

UCLASS(BlueprintType)
class STATEMACHINEEDITOR_API UStateMachineBlueprint : public UBlueprint
{
	GENERATED_UCLASS_BODY()

private:
	UPROPERTY()
	class UEdStateGraph* EdGraph;

public:
	UClass* GetBlueprintClass() const override;
	virtual bool SupportsInputEvents() const override;

	UEdStateGraph* StateMachineGraph() ;
};