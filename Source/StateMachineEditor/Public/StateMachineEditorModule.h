#include "IStateMachineEditorModule.h"
#include "StateMachine/Widgets/Nodes/EdStateNodeFactory.h"
#include "Compiler/StateMachineBlueprintCompiler.h"

class FStateMachineBlueprintCompiler;

class FStateMachineEditorModule: public IStateMachineEditorModule, public FGCObject
{

private:
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;
	TSharedPtr<FGraphPanelNodeFactory> GraphNodeFactory;
	FStateMachineBlueprintCompiler StateMachineBlueprintCompiler;

	static TSharedPtr<FKismetCompilerContext> GetCompiler(
		UBlueprint* BP, 
		FCompilerResultsLog& InMessageLog, 
		const FKismetCompilerOptions& InCompileOptions);

public:

	FStateMachineEditorModule();

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override;
	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;	
	virtual FStateMachineBlueprintCompiler* GetRegisteredCompiler() override;
};