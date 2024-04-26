#include "StateMachine/ProcStateMachineBlueprintEditor.h"
#include "StateMachine/ProcStateMachineBlueprint.h"
#include "StateMachine/Schema/ProcStateMachineGraphSchema.h"
#include "WidgetModeManager.h"
#include "StateMachine/ProcStateMachineBlueprintEditorToolbar.h"

#define LOCTEXT_NAMESPACE "PSM"

FProcStateMachineBlueprintEditor::FProcStateMachineBlueprintEditor(): FBlueprintEditor() {

}

FProcStateMachineBlueprintEditor::~FProcStateMachineBlueprintEditor() {
	UProcStateMachineBlueprint* Blueprint = Cast<UProcStateMachineBlueprint>(this->GetBlueprintObj());

	if (Blueprint)
	{
		Blueprint->OnChanged().RemoveAll(this);
		Blueprint->OnCompiled().RemoveAll(this);
	}

	FCoreUObjectDelegates::OnObjectsReplaced.RemoveAll(this);
}

void FProcStateMachineBlueprintEditor::InitProcStateMachineBlueprintEditor(
	const EToolkitMode::Type Mode, 
	const TSharedPtr< IToolkitHost >& InitToolkitHost, 
	const TArray<UBlueprint*>& InBlueprints, 
	bool bShouldOpenInDefaultsMode) 
{
	TSharedPtr<FProcStateMachineBlueprintEditor> ThisPtr(SharedThis(this));

	EditorToolbar = MakeShared<FProcStateMachineBlueprintEditorToolbar>(ThisPtr);

	InitBlueprintEditor(Mode, InitToolkitHost, InBlueprints, bShouldOpenInDefaultsMode);
}

//~ Begin FBlueprintEditor interface
void FProcStateMachineBlueprintEditor::Tick(float DeltaTime) {}
void FProcStateMachineBlueprintEditor::PostUndo(bool bSuccessful) {}
void FProcStateMachineBlueprintEditor::PostRedo(bool bSuccessful) {}
void FProcStateMachineBlueprintEditor::Compile() {}
//~ End FBlueprintEditor interface


//~ Begin FAssetEditorToolkit Interface
bool FProcStateMachineBlueprintEditor::OnRequestClose(EAssetEditorCloseReason InCloseReason) {
	return true;
}
// End of FAssetEditorToolkit 

//~ Begin FGCObjectInterface interface
void FProcStateMachineBlueprintEditor::AddReferencedObjects( FReferenceCollector& Collector ) {}
//~ End FGCObjectInterface interface

//~ Begin IToolkit interface
FName FProcStateMachineBlueprintEditor::GetToolkitContextFName() const { 
	return this->GetToolkitFName(); 
}

FName FProcStateMachineBlueprintEditor::GetToolkitFName() const { 
	return FName("ProcStateMachineBlueprintEditor");
}

FText FProcStateMachineBlueprintEditor::GetBaseToolkitName() const {
	return LOCTEXT("AppLabel", "ProcStateMachine Editor");
}

FString FProcStateMachineBlueprintEditor::GetWorldCentricTabPrefix() const {
	return LOCTEXT("WorldCentricTabPrefix", "StateMachine Editor").ToString();
}

FLinearColor FProcStateMachineBlueprintEditor::GetWorldCentricTabColorScale() const {
	return FLinearColor(0.3f, 0.25f, 0.35f, 0.5f);
}

void FProcStateMachineBlueprintEditor::InitToolMenuContext(FToolMenuContext& MenuContext) {

}
void FProcStateMachineBlueprintEditor::OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit) {

}

void FProcStateMachineBlueprintEditor::OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit) {

}
//~ End IToolkit interface


void FProcStateMachineBlueprintEditor::RegisterApplicationModes(const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode, bool bNewlyCreated/* = false*/)
{
	if (InBlueprints.Num() == 1)
	{
		TSharedPtr<FProcStateMachineBlueprintEditor> ThisPtr(SharedThis(this));

		// Create the modes and activate one (which will populate with a real layout)
		TArray<TSharedRef<FApplicationMode>> TempModeList;
		TempModeList.Add(MakeShared<FProcStateMachineGraphApplicationMode>(ThisPtr));
		TempModeList.Add(MakeShared<FProcStateMachineBlueprintApplicationMode>(ThisPtr));

		for (TSharedRef<FApplicationMode>& AppMode : TempModeList)
		{
			AddApplicationMode(AppMode->GetModeName(), AppMode);
		}

		SetCurrentMode(FProcStateMachineGraphApplicationMode::ModeName);
	}
	else
	{
		//// We either have no blueprints or many, open in the defaults mode for multi-editing
		//AddApplicationMode(
		//	FBlueprintEditorApplicationModes::BlueprintDefaultsMode,
		//	MakeShareable(new FBlueprintDefaultsApplicationMode(SharedThis(this))));
		//SetCurrentMode(FBlueprintEditorApplicationModes::BlueprintDefaultsMode);
	}
}

FGraphAppearanceInfo FProcStateMachineBlueprintEditor::GetGraphAppearance(class UEdGraph* InGraph) const {
	FGraphAppearanceInfo AppearanceInfo = FBlueprintEditor::GetGraphAppearance(InGraph);

	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText", "STATE MACHINE BLUEPRINT");

	/*
	if (FBlueprintEditorUtils::IsEditorUtilityBlueprint(GetBlueprintObj()))
	{
		AppearanceInfo.CornerText = LOCTEXT("EditorUtilityWidgetAppearanceCornerText", "EDITOR UTILITY WIDGET");
	}
	else if (GetBlueprintObj()->IsA(UWidgetBlueprint::StaticClass()))
	{
		AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText", "WIDGET BLUEPRINT");
	}
	*/

	return AppearanceInfo;
}

TSubclassOf<UEdGraphSchema> FProcStateMachineBlueprintEditor::GetDefaultSchemaClass() const {
	return UProcStateMachineGraphSchema::StaticClass();
}

void FProcStateMachineBlueprintEditor::CreateEditorModeManager()
{
	TSharedPtr<FWidgetModeManager> WidgetModeManager = MakeShared<FWidgetModeManager>();
	WidgetModeManager->OwningToolkit = SharedThis(this);
	EditorModeManager = WidgetModeManager;

}

void FProcStateMachineBlueprintEditor::InitalizeExtenders() {
	FBlueprintEditor::InitalizeExtenders();

	/*
	IUMGEditorModule& UMGEditorModule = FModuleManager::LoadModuleChecked<IUMGEditorModule>(
		"UMGEditor");
	AddMenuExtender(
		UMGEditorModule.GetMenuExtensibilityManager()->GetAllExtenders(
			GetToolkitCommands(), 
			GetEditingObjects()));

	AddMenuExtender(CreateMenuExtender());

	TArrayView<IUMGEditorModule::FWidgetEditorToolbarExtender> ToolbarExtenderDelegates 
		= UMGEditorModule.GetAllWidgetEditorToolbarExtenders();

	for (auto& ToolbarExtenderDelegate : ToolbarExtenderDelegates)
	{
		if (ToolbarExtenderDelegate.IsBound())
		{
			AddToolbarExtender(ToolbarExtenderDelegate.Execute(GetToolkitCommands(), SharedThis(this)));
		}
	}
	*/
}

#undef LOCTEXT_NAMESPACE