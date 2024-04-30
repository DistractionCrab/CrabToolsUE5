// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "UObject/TopLevelAssetPath.h"

extern const FName CrabToolsUE5EditorAppIdentifier;

/** The public interface of the UMG editor module. */
class ICrabToolsUE5EditorModule :
	public IModuleInterface,
	public IHasMenuExtensibility,
	public IHasToolBarExtensibility
{

public:
	static ICrabToolsUE5EditorModule& GetModule() {
		return FModuleManager::LoadModuleChecked<ICrabToolsUE5EditorModule>("CrabToolsUE5Editor");
	}
};