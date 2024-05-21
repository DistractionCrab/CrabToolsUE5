#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPin.h"
#include "StateMachine/StateMachine.h"
#include "EdBaseNode.generated.h"



/* Base Graph node to be used for states in the State Machine Graph. */
UCLASS(MinimalAPI)
class UEdBaseNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UEdBaseNode() {}
	virtual ~UEdBaseNode() {}


	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual void AllocateDefaultPins() override;
	virtual UEdGraphPin* GetInputPin() const { return Pins[0]; }
	virtual UEdGraphPin* GetOutputPin() const { return Pins[1]; }
};