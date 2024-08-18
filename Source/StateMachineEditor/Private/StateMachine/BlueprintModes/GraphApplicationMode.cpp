#include "StateMachine/BlueprintModes/GraphApplicationMode.h"

#include "BlueprintEditorTabs.h"
#include "SBlueprintEditorToolbar.h"
#include "BlueprintEditorSharedTabFactories.h"
#include "HAL/IConsoleManager.h"
#include "Internationalization/Internationalization.h"


#include "StateMachine/Editor.h"
#include "IStateMachineEditorModule.h"
#include "StateMachine/EditorToolbar.h"


// Tab Factory Imports
#include "StateMachine/TabFactory/GraphTabFactory.h"
#include "StateMachine/TabFactory/MachineDetailsTabFactory.h"
#include "StateMachine/TabFactory/GraphDetailsTabFactory.h"

const FName FGraphApplicationMode::ModeName("SMGraphEditorMode");


FGraphApplicationMode::FGraphApplicationMode(
	TSharedPtr<FEditor> InEditor)
:
	FBaseApplicationMode(		
		InEditor,
		FGraphApplicationMode::ModeName,
		FGraphApplicationMode::GetLocalizedMode)
{
	TSharedRef<FTabManager::FArea> MainArea = FTabManager::NewPrimaryArea()
		->SetOrientation(Orient_Vertical);


	MainArea->Split(
		FTabManager::NewSplitter()
		->SetSizeCoefficient(0.85f)
		->SetOrientation(Orient_Horizontal)
		->Split(
			FTabManager::NewStack()
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.2f)
			->AddTab(FGraphDetailsTabFactory::TabID, ETabState::OpenedTab)
		)
		->Split(
			FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
			->SetSizeCoefficient(0.9f)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.80f)
				->AddTab(FGraphTabFactory::TabID, ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.20f)
				->AddTab(FBlueprintEditorTabs::CompilerResultsID, ETabState::ClosedTab)
				->AddTab(FBlueprintEditorTabs::FindResultsID, ETabState::ClosedTab)
			)
		)
		->Split(
			FTabManager::NewStack()
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.3f)
			->AddTab(FMachineDetailsTabFactory::TabID, ETabState::OpenedTab)
		)
	);

	this->AddTabFactories(MyEditor.Pin());

	TabLayout = FTabManager::NewLayout("Editor_Graph_Layout_v1.0")
		->AddArea
		(
			MainArea
		);

	auto& Module = IStateMachineEditorModule::Get();
	ToolbarExtender = Module.GetToolBarExtensibilityManager()->GetAllExtenders();

	InEditor->GetWidgetToolbarBuilder()->AddEditorModesToolbar(ToolbarExtender);
	InEditor->RegisterModeToolbarIfUnregistered(GetModeName());

	FName OutParentToolbarName;
	FName ToolBarname = InEditor->GetToolMenuToolbarNameForMode(GetModeName(), OutParentToolbarName);

	if (UToolMenu* Toolbar = UToolMenus::Get()->FindMenu(ToolBarname))
	{
		InEditor->GetToolbarBuilder()->AddCompileToolbar(Toolbar);
		InEditor->GetToolbarBuilder()->AddDebuggingToolbar(Toolbar);
	}
}

#pragma region Initializers

void FGraphApplicationMode::AddTabFactories(
	TSharedPtr<FEditor> InProcStateMachineEditor) 
{
	TabFactories.RegisterFactory(
		MakeShareable(new FGraphDetailsTabFactory(InProcStateMachineEditor)));
	TabFactories.RegisterFactory(
		MakeShareable(new FGraphTabFactory(InProcStateMachineEditor)));
	TabFactories.RegisterFactory(
		MakeShareable(new FMachineDetailsTabFactory(InProcStateMachineEditor)));
}

#pragma endregion

void FGraphApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) {
	TSharedPtr<FEditor> BP = this->MyEditor.Pin();

	BP->RegisterToolbarTab(InTabManager.ToSharedRef());
	BP->PushTabFactories(TabFactories);
	BP->PushTabFactories(CoreTabFactories);
	BP->PushTabFactories(BlueprintEditorTabFactories);
}

void FGraphApplicationMode::PreDeactivateMode() {
	//FBlueprintEditorApplicationMode::PreDeactivateMode();
}

void FGraphApplicationMode::PostActivateMode() {
	// Do not do the following, otherwise an extra window will spawn.
	//FBlueprintEditorApplicationMode::PostActivateMode();
}


FText FGraphApplicationMode::GetLocalizedMode(const FName InMode) {

	if (InMode == FGraphApplicationMode::ModeName)
	{
		return NSLOCTEXT("ProcStateMachineBlueprintModes", "GraphMode", "Graph");	
	}
	
	return FText::GetEmpty();
}
