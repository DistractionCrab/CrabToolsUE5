#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Misc/NotifyHook.h"
#include "StateMachine/ProcStateMachineBlueprintEditor.h"

class IDetailsView;
class SBox;
class SEditableTextBox;

/**
 * The details view used in the designer section of the widget blueprint editor.
 */
class STATEMACHINEEDITOR_API SMachineDetailsView 
: public SCompoundWidget, public FNotifyHook, public FGCObject
{
public:
	SLATE_BEGIN_ARGS( SMachineDetailsView ){}
	SLATE_END_ARGS()

	void Construct(
		const FArguments& InArgs, 
		TSharedPtr<class FProcStateMachineBlueprintEditor> InBlueprintEditor);

	virtual FString GetReferencerName() const override
	{
		return TEXT("SMachineDetailsView");
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
};