#include "StateMachine/BlueprintModes/BlueprintApplicationMode.h"

#include "BlueprintEditorTabs.h"
#include "SBlueprintEditorToolbar.h"
#include "BlueprintEditorSharedTabFactories.h"
#include "HAL/IConsoleManager.h"
#include "Internationalization/Internationalization.h"

#include "StateMachine/EditorToolbar.h"
#include "StateMachine/TabFactory/GraphTabFactory.h"
#include "StateMachine/TabFactory/MachineDetailsTabFactory.h"
#include "IStateMachineEditorModule.h"

const FName FBlueprintApplicationMode::ModeName("PSMBlueprintEditorMode");


FBlueprintApplicationMode::FBlueprintApplicationMode(
	TSharedPtr<FEditor> InEditor)
: FBlueprintEditorApplicationMode(
	InEditor, 
	FBlueprintApplicationMode::ModeName,
	FBlueprintApplicationMode::GetLocalizedMode,
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
	auto& Module = IStateMachineEditorModule::Get();
	ToolbarExtender = Module.GetToolBarExtensibilityManager()->GetAllExtenders();

	InEditor->GetWidgetToolbarBuilder()->AddEditorModesToolbar(ToolbarExtender);
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

void FBlueprintApplicationMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) {
	TSharedPtr<FEditor> BP = this->MyEditor.Pin();


	BP->RegisterToolbarTab(InTabManager.ToSharedRef());
	BP->PushTabFactories(CoreTabFactories);
	BP->PushTabFactories(BlueprintEditorTabFactories);
	BP->PushTabFactories(TabFactories);
}

void FBlueprintApplicationMode::PreDeactivateMode() {

}

void FBlueprintApplicationMode::PostActivateMode() {
	FBlueprintEditorApplicationMode::PostActivateMode();
}


FText FBlueprintApplicationMode::GetLocalizedMode(const FName InMode) {

	if (InMode == FBlueprintApplicationMode::ModeName)
	{
		return NSLOCTEXT("BlueprintModes", "BlueprintMode", "Blueprint");	
	}
	
	return FText::GetEmpty();
}