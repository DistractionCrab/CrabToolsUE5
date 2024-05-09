#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "EdGraph/EdGraph.h"

#include "StateMachine/EdGraph/EdStateNode.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FNewStateAdded, UEdStateNode*);
DECLARE_MULTICAST_DELEGATE_OneParam(FInitEditor, UEdGraph*);

/* 
	Class to hold all event delegates for the editor. This will be used to
	communicate between different application modes, tabs, or other pieces.
*/
class FEditorCommandEvents {
public:
	FNewStateAdded NewStateEvent;
	FInitEditor InitEditor;
};