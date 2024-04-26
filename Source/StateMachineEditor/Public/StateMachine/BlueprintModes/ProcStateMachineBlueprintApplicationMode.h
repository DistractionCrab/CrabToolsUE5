#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "BlueprintEditorModes.h"

/////////////////////////////////////////////////////
// FWidgetGraphApplicationMode

class STATEMACHINEEDITOR_API FProcStateMachineBlueprintApplicationMode : public FBlueprintEditorApplicationMode
{
public:
	static const FName ModeName;

public:

	FProcStateMachineBlueprintApplicationMode(TSharedPtr<FProcStateMachineBlueprintEditor> InEditor);

	// FApplicationMode interface
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;
	// End of FApplicationMode interface

	static FText GetLocalizedMode(const FName InMode);

protected:
	TWeakPtr<class FProcStateMachineBlueprintEditor> MyEditor;

	// Set of spawnable tabs in the mode
	FWorkflowAllowedTabSet TabFactories;
};