#include "CrabNodesModule.h"
#include "EdGraphUtilities.h"
#include "Kismet/StateMachineGraphPanelPinFactory.h"

#define LOCTEXT_NAMESPACE "FCrabNodesModule"

const FName CrabNodesAppIdentifier = FName(TEXT("CrabNodesApp"));

FCrabNodesModule::FCrabNodesModule() 
{

}

void FCrabNodesModule::StartupModule() 
{
	auto BlueprintGraphPanelPinFactory = MakeShareable(new FStateMachineGraphPanelPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(BlueprintGraphPanelPinFactory);
}

void FCrabNodesModule::ShutdownModule()
{

}

void FCrabNodesModule::AddReferencedObjects(FReferenceCollector& Collector) {
	
}

FString FCrabNodesModule::GetReferencerName() const {
	return "CrabNodesModule";
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCrabNodesModule, CrabNodes)