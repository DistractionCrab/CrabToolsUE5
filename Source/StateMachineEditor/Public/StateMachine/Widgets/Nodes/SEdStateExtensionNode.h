#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPin.h"
#include "StateMachine/StateMachine.h"

class UEdStateExtensionNode;

class STATEMACHINEEDITOR_API SEdStateExtensionNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SEdStateExtensionNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdStateExtensionNode* InNode);
	
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual void CreatePinWidgets() override;
	virtual bool IsNameReadOnly() const override;
	virtual FSlateColor GetBorderBackgroundColor() const;
	virtual FSlateColor GetBackgroundColor() const;
	virtual EVisibility GetDragOverMarkerVisibility() const;
	virtual void UpdateGraphNode() override;
	virtual const FSlateBrush* GetNameIcon() const;	

	FName GetStateName() const;

private:

	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);
	void OnNodeNameChanged(FName OldName, FName Name);
};

