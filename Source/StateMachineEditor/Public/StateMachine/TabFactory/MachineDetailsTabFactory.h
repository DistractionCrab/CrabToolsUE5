#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"
#include "StateMachine/ProcStateMachineBlueprintEditor.h"

struct FMachineDetailsTabFactory : public FWorkflowTabFactory
{
public:
	static const FName TabID;
	
public:
	FMachineDetailsTabFactory(TSharedPtr<class FProcStateMachineBlueprintEditor> InBlueprintEditor);
	
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	
protected:
	TWeakPtr<class FProcStateMachineBlueprintEditor> BlueprintEditor;
};