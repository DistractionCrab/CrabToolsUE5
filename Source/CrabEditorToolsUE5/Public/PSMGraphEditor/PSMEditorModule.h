#include "Engine.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "UnrealEd.h"
#include "StateMachine/ProcStateMachine.h"

DECLARE_LOG_CATEGORY_EXTERN(PSMGraphEditor, All, All)

/**
 * The public interface to this module
 */
class FPSMEditorModule : public IModuleInterface
{

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};