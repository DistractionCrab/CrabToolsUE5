#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPin.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/EdGraph/EdBaseNode.h"
#include "EdStartStateNode.generated.h"



UCLASS(MinimalAPI)
class UEdStartStateNode : public UEdBaseStateNode
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="StateMachine")
	bool bCanOverride = false;

public:
	UEdStartStateNode();
	virtual ~UEdStartStateNode();

	virtual FName GetStateName() const override { return FName("~Start~"); }
	virtual FName GetNodeName() const override { return FName("~Start~"); }
	virtual bool CanUserDeleteNode() const override { return false; }
	virtual bool CanDuplicateNode() const override { return false; }

	bool CanOverride() const { return this->bCanOverride; }
};