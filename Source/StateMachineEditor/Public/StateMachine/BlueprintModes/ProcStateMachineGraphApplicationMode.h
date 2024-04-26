#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "StateMachine/ProcStateMachineBlueprintEditor.h"
#include "BlueprintEditorModes.h"
//#include "BlueprintModes/WidgetBlueprintApplicationMode.h"


class STATEMACHINEEDITOR_API FProcStateMachineGraphApplicationMode : public FBlueprintEditorApplicationMode
{

public:
	static const FName ModeName;

protected:
	// Set of spawnable tabs in the mode
	FWorkflowAllowedTabSet TabFactories;

	TWeakPtr<class FProcStateMachineBlueprintEditor> MyEditor;

public:
	FProcStateMachineGraphApplicationMode(
		TSharedPtr<FProcStateMachineBlueprintEditor> InPSMEditor);

	// FApplicationMode interface
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;
	// End of FApplicationMode interface

	static FText GetLocalizedMode(const FName InMode);

private:
	// Init Functions.
	void AddTabFactories(TSharedPtr<FProcStateMachineBlueprintEditor> InProcStateMachineEditor);
};