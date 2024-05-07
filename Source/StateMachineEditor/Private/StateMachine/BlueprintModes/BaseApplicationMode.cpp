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
	this->SetupToolbar(InEditor);
}


void FBaseApplicationMode::SetupToolbar(TSharedPtr<FEditor> InEditor) {
	auto& Module = IStateMachineEditorModule::Get();
	ToolbarExtender = Module.GetToolBarExtensibilityManager()->GetAllExtenders();

	InEditor->GetWidgetToolbarBuilder()->AddEditorModesToolbar(ToolbarExtender);
	InEditor->RegisterModeToolbarIfUnregistered(GetModeName());

	FName OutParentToolbarName;
	FName ToolBarname = InEditor->GetToolMenuToolbarNameForMode(GetModeName(), OutParentToolbarName);
	if (UToolMenu* Toolbar = UToolMenus::Get()->FindMenu(ToolBarname))
	{
		//InWidgetEditor->GetWidgetToolbarBuilder()->AddWidgetReflector(Toolbar);
		//InWidgetEditor->GetWidgetToolbarBuilder()->AddToolPalettes(Toolbar);
		InEditor->GetToolbarBuilder()->AddCompileToolbar(Toolbar);
		InEditor->GetToolbarBuilder()->AddDebuggingToolbar(Toolbar);
	}
}