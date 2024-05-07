#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPin.h"
#include "StateMachine/StateMachine.h"
#include "EdStateNode.generated.h"


class STATEMACHINEEDITOR_API SEdStateNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SEdStateNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdStateNode* InNode);

	/*
	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual bool IsNameReadOnly() const override;

	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);
	*/

	virtual FSlateColor GetBorderBackgroundColor() const;
	//virtual FSlateColor GetBackgroundColor() const;

	//virtual EVisibility GetDragOverMarkerVisibility() const;

	virtual const FSlateBrush* GetNameIcon() const;
	
};


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
	
	/*
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PrepareForCopying() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;

	virtual FLinearColor GetBackgroundColor() const;
	virtual UEdGraphPin* GetInputPin() const;
	virtual UEdGraphPin* GetOutputPin() const;

	#if WITH_EDITOR
		virtual void PostEditUndo() override;
	#endif
	*/

};