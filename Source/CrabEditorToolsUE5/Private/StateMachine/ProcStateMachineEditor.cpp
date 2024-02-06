#include "StateMachine/ProcStateMachineEditor.h"

void FPSMEditorModule::StartupModule() {
	ProcStateMachineAssetActions = MakeShared<FProcStateMachineAssetActions>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(ProcStateMachineAssetActions.ToSharedRef());
}

void FPSMEditorModule::ShutdownModule() {
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools")) return;
	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(ProcStateMachineAssetActions.ToSharedRef());
}


IMPLEMENT_MODULE(FPSMEditorModule, PSMGraphEditor)