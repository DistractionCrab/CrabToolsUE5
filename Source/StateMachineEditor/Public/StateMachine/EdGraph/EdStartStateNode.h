#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPin.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/EdGraph/EdBaseNode.h"
#include "EdStartStateNode.generated.h"



UCLASS(MinimalAPI)
class UEdStartStateNode : public UEdBaseNode
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FText Description;


public:
	UEdStartStateNode();
	virtual ~UEdStartStateNode();

	virtual bool CanUserDeleteNode() const override { return false; }
};