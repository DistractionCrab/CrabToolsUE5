// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrabToolsUE5EditorModule.h"
#include "Factory/InteractNavLinkProxyFactory.h"
#include "IPlacementModeModule.h"

#define LOCTEXT_NAMESPACE "FCrabToolsUE5EditorModule"

const FName CrabToolsUE5EditorAppIdentifier = FName(TEXT("CrabToolsUE5EditorApp"));


FCrabToolsUE5EditorModule::FCrabToolsUE5EditorModule() {

}

void FCrabToolsUE5EditorModule::StartupModule() 
{
	IPlacementModeModule& Module = IPlacementModeModule::Get();

	Module.RegisterPlaceableItem(
		FBuiltInPlacementCategories::Basic(),
		MakeShareable(new FPlaceableItem(*UInteractNavLinkProxyFactory::StaticClass(), 60))
	);
}

void FCrabToolsUE5EditorModule::ShutdownModule() 
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

FString FCrabToolsUE5EditorModule::GetReferencerName() const  {
	return "CrabToolsUE5EditorModule";
}




#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCrabToolsUE5EditorModule, CrabToolsUE5Editor)