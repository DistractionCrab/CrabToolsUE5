#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphNode.h"
#include "SGraphPin.h"
#include "StateMachine/StateMachine.h"

#include "StateMachine/EdGraph/EdStateNode.h"


class STATEMACHINEEDITOR_API SEdStateNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SEdStateNode) {}
	SLATE_END_ARGS()

	//DECLARE_MULTICAST_DELEGATE(FNodeStateUpdated, SEdStateNode*)

	//FNodeStateUpdated OnStateUpdated;

	void Construct(const FArguments& InArgs, UEdStateNode* InNode);

	/*
	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	

	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);
	*/

	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);
	//bool OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage);
	virtual bool IsNameReadOnly() const override;
	virtual FSlateColor GetBorderBackgroundColor() const;
	//virtual FSlateColor GetBackgroundColor() const;
	//virtual EVisibility GetDragOverMarkerVisibility() const;

	virtual const FSlateBrush* GetNameIcon() const;	
};

