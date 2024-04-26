#include "StateMachine/BlueprintModes/ProcStateMachineBlueprintApplicationMode.h"

#include "BlueprintEditorTabs.h"
#include "SBlueprintEditorToolbar.h"
#include "BlueprintEditorSharedTabFactories.h"
#include "HAL/IConsoleManager.h"
#include "Internationalization/Internationalization.h"

#include "StateMachine/ProcStateMachineBlueprintEditorToolbar.h"
#include "StateMachine/TabFactory/GraphTabFactory.h"
#include "StateMachine/TabFactory/MachineDetailsTabFactory.h"
#include "StateMachineEditorModule.h"

const FName FProcStateMachineBlueprintApplicationMode::ModeName("PSMBlueprintEditorMode");


FProcStateMachineBlueprintApplicationMode::FProcStateMachineBlueprintApplicationMode(
	TSharedPtr<FProcStateMachineBlueprintEditor> InEditor)
: FBlueprintEditorApplicationMode(
	InEditor, 
	FProcStateMachineBlueprintApplicationMode::ModeName,
	FProcStateMachineBlueprintApplicationMode::GetLocalizedMode,
	false,
	false),
MyEditor(InEditor)
{
	TabLayout = FTabManager::NewLayout("StateMachineBlueprintEditor_Graph_Layout_v2")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.15f)
					->Split
					(
						FTabManager::NewStack()->SetSizeCoefficient(0.5f)
						->AddTab(FBlueprintEditorTabs::MyBlueprintID, ETabState::OpenedTab)
					)
					->Split
					(
						FTabManager::NewStack()->SetSizeCoefficient(0.5f)
						->AddTab(FBlueprintEditorTabs::DetailsID, ETabState::OpenedTab)
					)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.70f)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.80f)
						->AddTab("Document", ETabState::ClosedTab)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.20f)
						->AddTab(FBlueprintEditorTabs::CompilerResultsID, ETabState::ClosedTab)
						->AddTab(FBlueprintEditorTabs::FindResultsID, ETabState::ClosedTab)
					)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.15f)
					->Split
					(
						FTabManager::NewStack()
						->AddTab(FBlueprintEditorTabs::PaletteID, ETabState::ClosedTab)
					)
				)
			)
		);

	// Initialize toolbar for this application mode.
	auto& Module = IStateMachineEditorModule::GetModule();
	ToolbarExtender = Module.GetToolBarExtensibilityManager()->GetAllExtenders();

	InEditor->GetWidgetToolbarBuilder()->AddProcStateMachineBlueprintEditorModesToolbar(ToolbarExtender);
	InEditor->RegisterModeToolbarIfUnregistered(GetModeName());

	FName OutParentToolbarName;
	FName ToolBarname = InEditor->GetToolMenuToolbarNameForMode(GetModeName(), OutParentToolbarName);
	if (UToolMenu* Toolbar = UToolMenus::Get()->FindMenu(ToolBarname))
	{
		InEditor->GetToolbarBuilder()->AddCompileToolbar(Toolbar);
		InEditor->GetToolbarBuilder()->AddScriptingToolbar(Toolbar);
		InEditor->GetToolbarBuilder()->AddBlueprintGlobalOptionsToolbar(Toolbar);
		InEditor->GetToolbarBuilder()->AddDebuggingToolbar(Toolbar);
	}
}

void FProcStateMachineBlueprintApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) {
	TSharedPtr<FProcStateMachineBlueprintEditor> BP = this->MyEditor.Pin();


	BP->RegisterToolbarTab(InTabManager.ToSharedRef());
	BP->PushTabFactories(CoreTabFactories);
	BP->PushTabFactories(BlueprintEditorTabFactories);
	BP->PushTabFactories(TabFactories);
}

void FProcStateMachineBlueprintApplicationMode::PreDeactivateMode() {

}

void FProcStateMachineBlueprintApplicationMode::PostActivateMode() {
	FBlueprintEditorApplicationMode::PostActivateMode();
}


FText FProcStateMachineBlueprintApplicationMode::GetLocalizedMode(const FName InMode) {

	if (InMode == FProcStateMachineBlueprintApplicationMode::ModeName)
	{
		return NSLOCTEXT("ProcStateMachineBlueprintModes", "BlueprintMode", "Blueprint");	
	}
	
	return FText::GetEmpty();
}