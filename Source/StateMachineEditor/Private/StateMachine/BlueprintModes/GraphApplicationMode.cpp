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
			FTabManager::NewStack()
			->SetHideTabWell(true)
			->SetSizeCoefficient(0.9f)
			->AddTab(FGraphTabFactory::TabID, ETabState::OpenedTab)			
		)
		->Split(
			FTabManager::NewStack()
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.2f)
			->AddTab(FMachineDetailsTabFactory::TabID, ETabState::OpenedTab)
		)
	);

	this->AddTabFactories(InEditor);

	TabLayout = FTabManager::NewLayout("Editor_Graph_Layout_v1.0")
		->AddArea
		(
			MainArea
		);
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

	//BP->RegisterToolbarTab(InTabManager.ToSharedRef());
	BP->PushTabFactories(TabFactories);
}

void FGraphApplicationMode::PreDeactivateMode() {
	//FBlueprintEditorApplicationMode::PreDeactivateMode();
}

void FGraphApplicationMode::PostActivateMode() {
	//FBlueprintEditorApplicationMode::PostActivateMode();
}


FText FGraphApplicationMode::GetLocalizedMode(const FName InMode) {

	if (InMode == FGraphApplicationMode::ModeName)
	{
		return NSLOCTEXT("ProcStateMachineBlueprintModes", "GraphMode", "Graph");	
	}
	
	return FText::GetEmpty();
}
