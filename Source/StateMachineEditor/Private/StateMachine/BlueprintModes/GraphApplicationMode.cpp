#include "StateMachine/BlueprintModes/GraphApplicationMode.h"

#include "BlueprintEditorTabs.h"
#include "SBlueprintEditorToolbar.h"
#include "BlueprintEditorSharedTabFactories.h"
#include "HAL/IConsoleManager.h"
#include "Internationalization/Internationalization.h"

#include "StateMachine/EditorToolbar.h"
#include "StateMachine/TabFactory/GraphTabFactory.h"
#include "StateMachine/TabFactory/MachineDetailsTabFactory.h"
#include "StateMachine/Editor.h"
#include "StateMachineEditorModule.h"

const FName FGraphApplicationMode::ModeName("PSMGraphEditorMode");


FGraphApplicationMode::FGraphApplicationMode(
	TSharedPtr<FEditor> InEditor)
: FBlueprintEditorApplicationMode(
	InEditor, 
	FGraphApplicationMode::ModeName,
	FGraphApplicationMode::GetLocalizedMode,
	false,
	false),
MyEditor(InEditor)
{
	TSharedRef<FTabManager::FArea> MainArea = FTabManager::NewPrimaryArea()
		->SetOrientation(Orient_Vertical);


	MainArea->Split(
		FTabManager::NewSplitter()
		->SetSizeCoefficient(0.85f)
		->SetOrientation(Orient_Horizontal)
		->Split(
			FTabManager::NewStack()
				->SetHideTabWell(true)
				->SetSizeCoefficient(0.2f)
				//->AddTab(FMachineDetailsTabFactory::TabID, ETabState::OpenedTab)
				->AddTab(FMachineDetailsTabFactory::TabID, ETabState::OpenedTab)
		)
		->Split(
			FTabManager::NewStack()
				->SetHideTabWell(true)
				->SetSizeCoefficient(0.2f)
				->AddTab(FGraphTabFactory::TabID, ETabState::OpenedTab)
		)
	);

	this->AddTabFactories(InEditor);

	TabLayout = FTabManager::NewLayout("Editor_Graph_Layout_v1.0")
		->AddArea
		(
			MainArea
		);

	auto& Module = IStateMachineEditorModule::GetModule();
	ToolbarExtender = Module.GetToolBarExtensibilityManager()->GetAllExtenders();

	InEditor->GetWidgetToolbarBuilder()->AddEditorModesToolbar(ToolbarExtender);
	InEditor->RegisterModeToolbarIfUnregistered(GetModeName());
}

#pragma region Initializers

void FGraphApplicationMode::AddTabFactories(
	TSharedPtr<FEditor> InProcStateMachineEditor) 
{
	TabFactories.RegisterFactory(
		MakeShareable(new FGraphTabFactory(InProcStateMachineEditor)));
	TabFactories.RegisterFactory(
		MakeShareable(new FMachineDetailsTabFactory(InProcStateMachineEditor)));
}

#pragma endregion

void FGraphApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) {
	TSharedPtr<FEditor> BP = this->MyEditor.Pin();

	//BP->RegisterToolbarTab(InTabManager.ToSharedRef());
	BP->PushTabFactories(TabFactories);
}

void FGraphApplicationMode::PreDeactivateMode() {

}

void FGraphApplicationMode::PostActivateMode() {

}


FText FGraphApplicationMode::GetLocalizedMode(const FName InMode) {

	if (InMode == FGraphApplicationMode::ModeName)
	{
		return NSLOCTEXT("ProcStateMachineBlueprintModes", "GraphMode", "Graph");	
	}
	
	return FText::GetEmpty();
}