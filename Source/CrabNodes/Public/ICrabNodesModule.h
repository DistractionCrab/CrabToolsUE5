// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "UObject/TopLevelAssetPath.h"

extern const FName CrabNodesAppIdentifier;

class ICrabNodesModule : public IModuleInterface
{

public:
	static ICrabNodesModule& Get()
	{
		return FModuleManager::LoadModuleChecked<ICrabNodesModule>("CrabNodes");
	}
};