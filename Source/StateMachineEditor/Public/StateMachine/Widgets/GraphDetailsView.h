#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Misc/NotifyHook.h"
#include "StateMachine/Editor.h"

class IDetailsView;
class SBox;
class SEditableTextBox;

/**
 * The tab which shows details about selected features of the State Machine's graph,
 * such as Nodes and Edges.
 */
class STATEMACHINEEDITOR_API SGraphDetailsView 
: public SCompoundWidget, public FNotifyHook, public FGCObject
{
public:
	SLATE_BEGIN_ARGS(SGraphDetailsView){}
	SLATE_END_ARGS()

	void Construct(
		const FArguments& InArgs, 
		TSharedPtr<class FEditor> InBlueprintEditor);

	virtual FString GetReferencerName() const override
	{
		return TEXT("SNodeDetailsView");
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
};