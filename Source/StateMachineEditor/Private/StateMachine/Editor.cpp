#include "StateMachine/Editor.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "StateMachine/EdGraph/StateMachineSchema.h"
#include "WidgetModeManager.h"
#include "StateMachine/EditorToolbar.h"
#include "StateMachine/BlueprintModes/GraphApplicationMode.h"
#include "StateMachine/BlueprintModes/BlueprintApplicationMode.h"


#define LOCTEXT_NAMESPACE "StateMachineEditor"

FEditor::FEditor(): FBlueprintEditor()
{
	
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
void FEditor::Compile()
{
	UE_LOG(LogTemp, Warning, TEXT("Compilation has started?"));
}
//~ End FBlueprintEditor interface


//~ Begin FAssetEditorToolkit Interface
bool FEditor::OnRequestClose(EAssetEditorCloseReason InCloseReason) {
	this->GetStateMachineBlueprintObj()->StateMachineGraph()->ClearEvents();
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
	return FName("StateMachineEditor");
}

FText FEditor::GetBaseToolkitName() const {
	return LOCTEXT("AppLabel", "StateMachineEditor");
}

FString FEditor::GetWorldCentricTabPrefix() const {
	return LOCTEXT("WorldCentricTabPrefix", "State Machine Editor").ToString();
}

FLinearColor FEditor::GetWorldCentricTabColorScale() const {
	return FLinearColor(0.3f, 0.25f, 0.35f, 0.5f);
}

void FEditor::InitToolMenuContext(FToolMenuContext& MenuContext) {
	FBlueprintEditor::InitToolMenuContext(MenuContext);
}
void FEditor::OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit) {
	//FBlueprintEditor::OnToolkitHostingStarted(Toolkit);
}

void FEditor::OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit) {
	//FBlueprintEditor::OnToolkitHostingFinished(Toolkit);
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
}

FGraphAppearanceInfo FEditor::GetGraphAppearance(class UEdGraph* InGraph) const {
	FGraphAppearanceInfo AppearanceInfo = FBlueprintEditor::GetGraphAppearance(InGraph);

	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText", "STATE MACHINE BLUEPRINT");

	return AppearanceInfo;
}

TSubclassOf<UEdGraphSchema> FEditor::GetDefaultSchemaClass() const {
	return UStateMachineSchema::StaticClass();
}

void FEditor::CreateEditorModeManager()
{
	TSharedPtr<FWidgetModeManager> WidgetModeManager = MakeShared<FWidgetModeManager>();
	WidgetModeManager->OwningToolkit = SharedThis(this);
	EditorModeManager = WidgetModeManager;

}

void FEditor::InitalizeExtenders() {
	FBlueprintEditor::InitalizeExtenders();
}

TSharedPtr<FExtender> FEditor::CreateMenuExtender()
{
	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender);

	return MenuExtender;
}

UStateMachineBlueprint* FEditor::GetStateMachineBlueprintObj() {
	return Cast<UStateMachineBlueprint>(FBlueprintEditor::GetBlueprintObj());
}

#undef LOCTEXT_NAMESPACE