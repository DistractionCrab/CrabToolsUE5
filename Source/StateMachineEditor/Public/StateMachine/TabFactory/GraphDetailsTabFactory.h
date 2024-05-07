#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"
#include "StateMachine/Editor.h"

struct FGraphDetailsTabFactory : public FWorkflowTabFactory
{
public:
	static const FName TabID;
	
public:
	FGraphDetailsTabFactory(TSharedPtr<class FEditor> InBlueprintEditor);
	
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	
protected:
	TWeakPtr<class FEditor> BlueprintEditor;
};