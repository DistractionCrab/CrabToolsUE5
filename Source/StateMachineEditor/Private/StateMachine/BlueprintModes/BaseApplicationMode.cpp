#include "StateMachine/BlueprintModes/BaseApplicationMode.h"

#include "BlueprintEditorTabs.h"
#include "SBlueprintEditorToolbar.h"
#include "BlueprintEditorSharedTabFactories.h"
#include "HAL/IConsoleManager.h"
#include "Internationalization/Internationalization.h"


#include "StateMachine/Editor.h"
#include "IStateMachineEditorModule.h"
#include "StateMachine/EditorToolbar.h"


FBaseApplicationMode::FBaseApplicationMode(
	TSharedPtr<FEditor> InEditor,
	FName ModeName,
	FText(*GetLocalizedMode)(const FName))
: FBlueprintEditorApplicationMode(
		InEditor,
		ModeName,
		GetLocalizedMode,
		false,
		false
	),
	MyEditor(InEditor)
{
}
