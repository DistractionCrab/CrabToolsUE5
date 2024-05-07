#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "StateMachine/BlueprintModes/BaseApplicationMode.h"


class STATEMACHINEEDITOR_API FBlueprintApplicationMode : public FBaseApplicationMode
{
public:
	static const FName ModeName;

protected:
	// Set of spawnable tabs in the mode
	FWorkflowAllowedTabSet TabFactories;

public:

	FBlueprintApplicationMode(TSharedPtr<class FEditor> InEditor);

	// FApplicationMode interface
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;
	// End of FApplicationMode interface

	static FText GetLocalizedMode(const FName InMode);

protected:
	virtual void SetupToolbar(TSharedPtr<FEditor> InEditor) override;
};