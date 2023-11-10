#pragma once

#include "CoreMinimal.h"
#include "StateMachine/ProcSateMachine.h"

#if ENGINE_MAJOR_VERSION == 5
	#include "UObject/ObjectSaveContext.h"
#endif // #if ENGINE_MAJOR_VERSION == 5

class CRABTOOLSUE5_API FAssetEditor_PSMGraph: public FAssetEditorToolkit, public FNotifyHook, public FGCObject {


public:
	FAssetEditor_PSMGraph();
	virtual ~FAssetEditor_PSMGraph();

	void InitPSMGraphEditorAssetEditor(
		const EToolkitMode::Type Mode, 
		const TSharedPtr< IToolkitHost >& InitToolkitHost, 
		UProcStateMachine* Graph);
}