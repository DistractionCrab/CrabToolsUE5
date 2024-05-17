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
 * The details view used in the designer section of the widget blueprint editor.
 */
class STATEMACHINEEDITOR_API SMachineDetailsView 
: public SCompoundWidget, public FNotifyHook, public FGCObject
{
private:
	TSharedPtr<class SKismetInspector> Inspector;

public:
	SLATE_BEGIN_ARGS( SMachineDetailsView ){}
	SLATE_END_ARGS()

	void Construct(
		const FArguments& InArgs, 
		TSharedPtr<class FEditor> InBlueprintEditor);

	virtual FString GetReferencerName() const override
	{
		return TEXT("SMachineDetailsView");
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

private:
	void OnGraphChanged(const FEdGraphEditAction& Action);
	void OnSelectionChanged(TArray<class UEdStateNode*>& SelectedNodes);
	void BindEvents(TSharedPtr<class FEditor> InEditor);
};