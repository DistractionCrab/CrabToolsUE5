#include "StateMachineEditorModule.h"

#define LOCTEXT_NAMESPACE "FStateMachineEditorModule"

const FName StateMachineEditorAppIdentifier = FName(TEXT("StateMachineEditorApp"));

FStateMachineEditorModule::FStateMachineEditorModule() 
{

}

void FStateMachineEditorModule::StartupModule() 
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	MenuExtensibilityManager = MakeShared<FExtensibilityManager>();
	ToolBarExtensibilityManager = MakeShared<FExtensibilityManager>();
}

void FStateMachineEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}


TSharedPtr<FExtensibilityManager> FStateMachineEditorModule::GetMenuExtensibilityManager() {
	return MenuExtensibilityManager; 
}

TSharedPtr<FExtensibilityManager> FStateMachineEditorModule::GetToolBarExtensibilityManager() 
{ 
	return ToolBarExtensibilityManager; 
}

void FStateMachineEditorModule::AddReferencedObjects(FReferenceCollector& Collector) {
	
}

FString FStateMachineEditorModule::GetReferencerName() const {
	return "StateMachineEditorModule";
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FStateMachineEditorModule, StateMachineEditor)