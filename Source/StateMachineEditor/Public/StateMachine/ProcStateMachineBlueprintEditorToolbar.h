#pragma once

#include "CoreMinimal.h"

class FExtender;
class FToolBarBuilder;
class FProcStateMachineBlueprintEditor;
class UToolMenu;

/**
 * Handles all of the toolbar related construction for the widget blueprint editor.
 */
class FProcStateMachineBlueprintEditorToolbar
: public TSharedFromThis<FProcStateMachineBlueprintEditorToolbar>
{

public:
	/** Constructor */
	FProcStateMachineBlueprintEditorToolbar(
		TSharedPtr<FProcStateMachineBlueprintEditor>& InEditor);
	
	/**
	 * Builds the modes toolbar for the widget blueprint editor.
	 */
	void AddProcStateMachineBlueprintEditorModesToolbar(TSharedPtr<FExtender> Extender);

	void AddWidgetReflector(UToolMenu* InMenu);

	void AddToolPalettes(UToolMenu* InMenu);

	void FillProcStateMachineBlueprintEditorModesToolbar(FToolBarBuilder& ToolbarBuilder);

	TWeakPtr<FProcStateMachineBlueprintEditor> MyEditor;
};