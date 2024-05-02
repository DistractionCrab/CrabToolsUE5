#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "BlueprintEditorModes.h"

/////////////////////////////////////////////////////
// FWidgetGraphApplicationMode

class STATEMACHINEEDITOR_API FBlueprintApplicationMode : public FBlueprintEditorApplicationMode
{
public:
	static const FName ModeName;

public:

	FBlueprintApplicationMode(TSharedPtr<class FEditor> InEditor);

	// FApplicationMode interface
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;
	// End of FApplicationMode interface

	static FText GetLocalizedMode(const FName InMode);

protected:
	TWeakPtr<class FEditor> MyEditor;

	// Set of spawnable tabs in the mode
	FWorkflowAllowedTabSet TabFactories;
};