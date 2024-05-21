#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPin.h"
#include "StateMachine/StateMachine.h"
#include "StateMachine/Widgets/Nodes/SEdStateNode.h"
#include "StateMachine/EdGraph/EdStartStateNode.h"



class STATEMACHINEEDITOR_API SEdStartStateNode : public SEdStateNode
{
public:
	SLATE_BEGIN_ARGS(SEdStartStateNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdStartStateNode* InNode);

	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual void CreatePinWidgets() override;
	virtual bool IsNameReadOnly() const override;
	virtual FSlateColor GetBorderBackgroundColor() const;
	virtual FSlateColor GetBackgroundColor() const;
	virtual EVisibility GetDragOverMarkerVisibility() const;

	virtual const FSlateBrush* GetNameIcon() const;
};

