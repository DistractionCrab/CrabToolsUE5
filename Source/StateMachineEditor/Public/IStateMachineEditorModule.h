// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "UObject/TopLevelAssetPath.h"

extern const FName StateMachineEditorAppIdentifier;

/** The public interface of the UMG editor module. */
class IStateMachineEditorModule :
	public IModuleInterface,
	public IHasMenuExtensibility,
	public IHasToolBarExtensibility
{

public:
	static IStateMachineEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked<IStateMachineEditorModule>("StateMachineEditor");
	}
};