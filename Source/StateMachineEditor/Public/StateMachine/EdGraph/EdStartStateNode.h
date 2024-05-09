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

public:
	UEdStartStateNode();
	virtual ~UEdStartStateNode();

	//virtual void AllocateDefaultPins() override;
	//virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	//virtual void PrepareForCopying() override;
	//virtual void AutowireNewNode(UEdGraphPin* FromPin) override;

	//virtual FLinearColor GetBackgroundColor() const;
	//virtual UEdGraphPin* GetInputPin() const;
	//virtual UEdGraphPin* GetOutputPin() const;

	#if WITH_EDITOR
		//virtual void PostEditUndo() override;
	#endif

};