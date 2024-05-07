#include "IStateMachineEditorModule.h"
#include "StateMachine/Widgets/Nodes/EdStateNodeFactory.h"


class FStateMachineEditorModule: public IStateMachineEditorModule, public FGCObject
{

private:
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;
	TSharedPtr<FGraphPanelNodeFactory> GraphNodeFactory;

public:

	FStateMachineEditorModule();

	virtual void StartupModule() override;

	virtual void ShutdownModule() override;


	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override;
	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override;

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	virtual FString GetReferencerName() const override;
};