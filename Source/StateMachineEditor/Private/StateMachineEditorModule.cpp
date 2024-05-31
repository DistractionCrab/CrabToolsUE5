#include "StateMachineEditorModule.h"
#include "EdGraphUtilities.h"
#include "Compiler/StateMachineBlueprintCompiler.h"
#include "StateMachine/StateMachineBlueprint.h"

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
	GraphNodeFactory = MakeShareable(new FEdStateNodeFactory());


	FEdGraphUtilities::RegisterVisualNodeFactory(GraphNodeFactory);


	// Register widget blueprint compiler we do this no matter what.
	IKismetCompilerInterface& KismetCompilerModule 
		= FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
	KismetCompilerModule.GetCompilers().Add(&StateMachineBlueprintCompiler);
	KismetCompilerModule.OverrideBPTypeForClass(
		UStateMachine::StaticClass(), UStateMachineBlueprint::StaticClass());

	FKismetCompilerContext::RegisterCompilerForBP(
		UStateMachineBlueprint::StaticClass(), 
		&FStateMachineEditorModule::GetCompiler);
}

void FStateMachineEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	if (auto KismetCompilerModule = FModuleManager::GetModulePtr<IKismetCompilerInterface>("KismetCompiler"))
	{
		KismetCompilerModule->GetCompilers().Remove(&StateMachineBlueprintCompiler);
	}
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

TSharedPtr<FKismetCompilerContext> FStateMachineEditorModule::GetCompiler(
	UBlueprint* BP, 
	FCompilerResultsLog& InMessageLog, 
	const FKismetCompilerOptions& InCompileOptions)
{
	return TSharedPtr<FKismetCompilerContext>(
		new FStateMachineBlueprintCompilerContext(
			CastChecked<UStateMachineBlueprint>(BP), 
			InMessageLog, 
			InCompileOptions));
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FStateMachineEditorModule, StateMachineEditor)