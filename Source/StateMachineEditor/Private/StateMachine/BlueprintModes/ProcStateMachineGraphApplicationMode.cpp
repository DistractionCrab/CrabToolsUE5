#include "StateMachine/BlueprintModes/ProcStateMachineGraphApplicationMode.h"

#include "BlueprintEditorTabs.h"
#include "SBlueprintEditorToolbar.h"
#include "BlueprintEditorSharedTabFactories.h"
#include "HAL/IConsoleManager.h"
#include "Internationalization/Internationalization.h"

#include "StateMachine/ProcStateMachineBlueprintEditorToolbar.h"
#include "StateMachine/TabFactory/GraphTabFactory.h"
#include "StateMachine/TabFactory/MachineDetailsTabFactory.h"
#include "StateMachine/ProcStateMachineBlueprintEditor.h"
#include "StateMachineEditorModule.h"

const FName FProcStateMachineGraphApplicationMode::ModeName("PSMGraphEditorMode");


FProcStateMachineGraphApplicationMode::FProcStateMachineGraphApplicationMode(
	TSharedPtr<FProcStateMachineBlueprintEditor> InEditor)
: FBlueprintEditorApplicationMode(
	InEditor, 
	FProcStateMachineGraphApplicationMode::ModeName,
	FProcStateMachineGraphApplicationMode::GetLocalizedMode,
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

	TabLayout = FTabManager::NewLayout("ProcStateMachineBlueprintEditor_Graph_Layout_v1.0")
		->AddArea
		(
			MainArea
		);

	auto& Module = IStateMachineEditorModule::GetModule();
	ToolbarExtender = Module.GetToolBarExtensibilityManager()->GetAllExtenders();

	InEditor->GetWidgetToolbarBuilder()->AddProcStateMachineBlueprintEditorModesToolbar(ToolbarExtender);
	InEditor->RegisterModeToolbarIfUnregistered(GetModeName());
}

#pragma region Initializers

void FProcStateMachineGraphApplicationMode::AddTabFactories(
	TSharedPtr<FProcStateMachineBlueprintEditor> InProcStateMachineEditor) 
{
	TabFactories.RegisterFactory(
		MakeShareable(new FGraphTabFactory(InProcStateMachineEditor)));
	TabFactories.RegisterFactory(
		MakeShareable(new FMachineDetailsTabFactory(InProcStateMachineEditor)));
}

#pragma endregion

void FProcStateMachineGraphApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) {
	TSharedPtr<FProcStateMachineBlueprintEditor> BP = this->MyEditor.Pin();

	//BP->RegisterToolbarTab(InTabManager.ToSharedRef());
	BP->PushTabFactories(TabFactories);
}

void FProcStateMachineGraphApplicationMode::PreDeactivateMode() {

}

void FProcStateMachineGraphApplicationMode::PostActivateMode() {

}


FText FProcStateMachineGraphApplicationMode::GetLocalizedMode(const FName InMode) {

	if (InMode == FProcStateMachineGraphApplicationMode::ModeName)
	{
		return NSLOCTEXT("ProcStateMachineBlueprintModes", "GraphMode", "Graph");	
	}
	
	return FText::GetEmpty();
}