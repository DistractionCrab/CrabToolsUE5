// Copyright Epic Games, Inc. All Rights Reserved.

#include "StateMachineEditorModule.h"

#define LOCTEXT_NAMESPACE "FStateMachineEditorModule"

const FName StateMachineEditorAppIdentifier = FName(TEXT("StateMachineEditorApp"));

class FStateMachineEditorModule: public IStateMachineEditorModule, public FGCObject
{

private:
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;

public:

	FStateMachineEditorModule() {}

	virtual void StartupModule() override
	{
		// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
		MenuExtensibilityManager = MakeShared<FExtensibilityManager>();
		ToolBarExtensibilityManager = MakeShared<FExtensibilityManager>();
	}

	virtual void ShutdownModule() override
	{
		// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
		// we call this function before unloading the module.
	}


	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override { return MenuExtensibilityManager; }
	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override { return ToolBarExtensibilityManager; }

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override {
		
	}

	virtual FString GetReferencerName() const override {
		return "StateMachineEditorModule";
	}


};



#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FStateMachineEditorModule, StateMachineEditor)