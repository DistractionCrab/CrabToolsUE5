#pragma once

#include "CoreMinimal.h"

class FExtender;
class FToolBarBuilder;
class FEditor;
class UToolMenu;

/**
 * Handles all of the toolbar related construction for the widget blueprint editor.
 */
class FEditorToolbar
: public TSharedFromThis<FEditorToolbar>
{

public:
	/** Constructor */
	FEditorToolbar(
		TSharedPtr<FEditor>& InEditor);
	
	/**
	 * Builds the modes toolbar for the widget blueprint editor.
	 */
	void AddEditorModesToolbar(TSharedPtr<FExtender> Extender);
	void FillEditorModesToolbar(FToolBarBuilder& ToolbarBuilder);

	TWeakPtr<FEditor> MyEditor;
};