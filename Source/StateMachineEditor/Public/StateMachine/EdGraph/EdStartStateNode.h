#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPin.h"
#include "StateMachine/StateMachine.h"
#include "EdStartStateNode.generated.h"



UCLASS(MinimalAPI)
class UEdStartStateNode : public UEdGraphNode
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FText Description;


public:
	UEdStartStateNode(const FObjectInitializer& ObjectInitializer);
	UEdStartStateNode();
	virtual ~UEdStartStateNode();

	//virtual void AllocateDefaultPins() override;
	//virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	//virtual void PrepareForCopying() override;
	//virtual void AutowireNewNode(UEdGraphPin* FromPin) override;

	//virtual FLinearColor GetBackgroundColor() const;
	//virtual UEdGraphPin* GetInputPin() const;
	//virtual UEdGraphPin* GetOutputPin() const;

	
	virtual bool CanUserDeleteNode() const override { return false; }

};