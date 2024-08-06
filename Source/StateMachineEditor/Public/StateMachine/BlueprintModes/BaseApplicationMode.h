#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "StateMachine/Editor.h"
#include "BlueprintEditorModes.h"



class STATEMACHINEEDITOR_API FBaseApplicationMode : public FBlueprintEditorApplicationMode
{

protected:
		// Reference to owning editor.
	TWeakPtr<class FEditor> MyEditor;

public:
	FBaseApplicationMode(
		TSharedPtr<FEditor> InEditor,
		FName ModeName,
		FText(*GetLocalizedMode)(const FName));
};