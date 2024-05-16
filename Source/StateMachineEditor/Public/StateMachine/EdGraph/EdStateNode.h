#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPin.h"
#include "StateMachine/StateMachine.h"
#include "EdStateNode.generated.h"



UCLASS(MinimalAPI)
class UEdStateNode : public UEdGraphNode
{
	GENERATED_BODY()

	/* Whether or not a blueprint variable should be generated for this state. */
	UPROPERTY(EditDefaultsOnly, Category="StateMachineGraph")
	bool bIsVariable;

	UPROPERTY(VisibleAnywhere, Category = "StateMachineGraph")
	FName StateName;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "StateMachineGraph")
	TArray<TObjectPtr<UStateNode>> Nodes;

public:
	UEdStateNode();
	virtual ~UEdStateNode();

	virtual TSubclassOf<UStateNode> GetNodeClass() const;

	void SetNodeTemplate(UStateNode* NewNode) { this->Nodes.Add(NewNode); }

	FName GetStateName() const { return this->StateName; }
	void SetStateName(FName NewName) { this->StateName = NewName; }

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