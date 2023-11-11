#pragma once

#include "CoreMinimal.h"
#include "StateMachine/ProcStateMachine.h"

#if ENGINE_MAJOR_VERSION == 5
	#include "UObject/ObjectSaveContext.h"
#endif // #if ENGINE_MAJOR_VERSION == 5



class CRABEDITORTOOLSUE5_API FPSMGraphEditor: public FAssetEditorToolkit, public FNotifyHook, public FGCObject {
public:
	FPSMGraphEditor();
	virtual ~FPSMGraphEditor();

	
	void InitPSMGraphEditorEditor(
		const EToolkitMode::Type Mode, 
		const TSharedPtr< IToolkitHost >& InitToolkitHost, 
		UProcStateMachine* Graph);
	
};
