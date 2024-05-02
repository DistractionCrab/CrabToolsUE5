#include "StateMachine/Editor.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "StateMachine/Schema/GraphSchema.h"
#include "WidgetModeManager.h"
#include "StateMachine/EditorToolbar.h"
#include "StateMachine/BlueprintModes/GraphApplicationMode.h"
#include "StateMachine/BlueprintModes/BlueprintApplicationMode.h"

#define LOCTEXT_NAMESPACE "PSM"

FEditor::FEditor(): FBlueprintEditor() {

}

FEditor::~FEditor() {
	UStateMachineBlueprint* Blueprint = Cast<UStateMachineBlueprint>(this->GetBlueprintObj());

	if (Blueprint)
	{
		Blueprint->OnChanged().RemoveAll(this);
		Blueprint->OnCompiled().RemoveAll(this);
	}

	FCoreUObjectDelegates::OnObjectsReplaced.RemoveAll(this);
}

void FEditor::InitEditor(
	const EToolkitMode::Type Mode, 
	const TSharedPtr< IToolkitHost >& InitToolkitHost, 
	const TArray<UBlueprint*>& InBlueprints, 
	bool bShouldOpenInDefaultsMode) 
{
	TSharedPtr<FEditor> ThisPtr(SharedThis(this));

	EditorToolbar = MakeShared<FEditorToolbar>(ThisPtr);

	InitBlueprintEditor(Mode, InitToolkitHost, InBlueprints, bShouldOpenInDefaultsMode);
}

//~ Begin FBlueprintEditor interface
void FEditor::Tick(float DeltaTime) {}
void FEditor::PostUndo(bool bSuccessful) {}
void FEditor::PostRedo(bool bSuccessful) {}
void FEditor::Compile() {}
//~ End FBlueprintEditor interface


//~ Begin FAssetEditorToolkit Interface
bool FEditor::OnRequestClose(EAssetEditorCloseReason InCloseReason) {
	return true;
}
// End of FAssetEditorToolkit 

//~ Begin FGCObjectInterface interface
void FEditor::AddReferencedObjects( FReferenceCollector& Collector ) {}
//~ End FGCObjectInterface interface

//~ Begin IToolkit interface
FName FEditor::GetToolkitContextFName() const { 
	return this->GetToolkitFName(); 
}

FName FEditor::GetToolkitFName() const { 
	return FName("Editor");
}

FText FEditor::GetBaseToolkitName() const {
	return LOCTEXT("AppLabel", "ProcStateMachine Editor");
}

FString FEditor::GetWorldCentricTabPrefix() const {
	return LOCTEXT("WorldCentricTabPrefix", "StateMachine Editor").ToString();
}

FLinearColor FEditor::GetWorldCentricTabColorScale() const {
	return FLinearColor(0.3f, 0.25f, 0.35f, 0.5f);
}

void FEditor::InitToolMenuContext(FToolMenuContext& MenuContext) {

}
void FEditor::OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit) {

}

void FEditor::OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit) {

}
//~ End IToolkit interface


void FEditor::RegisterApplicationModes(const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode, bool bNewlyCreated/* = false*/)
{
	if (InBlueprints.Num() == 1)
	{
		TSharedPtr<FEditor> ThisPtr(SharedThis(this));

		// Create the modes and activate one (which will populate with a real layout)
		TArray<TSharedRef<FApplicationMode>> TempModeList;
		TempModeList.Add(MakeShared<FGraphApplicationMode>(ThisPtr));
		TempModeList.Add(MakeShared<FBlueprintApplicationMode>(ThisPtr));

		for (TSharedRef<FApplicationMode>& AppMode : TempModeList)
		{
			AddApplicationMode(AppMode->GetModeName(), AppMode);
		}

		SetCurrentMode(FGraphApplicationMode::ModeName);
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

FGraphAppearanceInfo FEditor::GetGraphAppearance(class UEdGraph* InGraph) const {
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

TSubclassOf<UEdGraphSchema> FEditor::GetDefaultSchemaClass() const {
	return UGraphSchema::StaticClass();
}

void FEditor::CreateEditorModeManager()
{
	TSharedPtr<FWidgetModeManager> WidgetModeManager = MakeShared<FWidgetModeManager>();
	WidgetModeManager->OwningToolkit = SharedThis(this);
	EditorModeManager = WidgetModeManager;

}

void FEditor::InitalizeExtenders() {
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