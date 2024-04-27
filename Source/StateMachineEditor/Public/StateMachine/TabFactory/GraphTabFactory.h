#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"
#include "StateMachine/Editor.h"

struct FGraphTabFactory : public FWorkflowTabFactory
{
public:
	static const FName ToolPaletteTabID;
	static const FName TabID;
	
public:
	FGraphTabFactory(TSharedPtr<class FEditor> InBlueprintEditor);
	
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	
protected:
	TWeakPtr<class FEditor> BlueprintEditor;
};