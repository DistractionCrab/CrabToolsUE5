#include "ICrabNodesModule.h"

class FStateMachineBlueprintCompiler;

class FCrabNodesModule: public ICrabNodesModule, public FGCObject
{
private:

	//TSharedPtr<FGraphPanelNodeFactory> GraphNodeFactory;

public:

	FCrabNodesModule();

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;	
};