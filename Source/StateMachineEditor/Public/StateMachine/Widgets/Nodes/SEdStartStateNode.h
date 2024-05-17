#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPin.h"
#include "StateMachine/StateMachine.h"
#include "STateMachine/EdGraph/EdStartStateNode.h"

class STATEMACHINEEDITOR_API SEdStartStateNode : public SGraphNode
{
private:
	
public:
	SLATE_BEGIN_ARGS(SEdStartStateNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdStartStateNode* InNode);

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

