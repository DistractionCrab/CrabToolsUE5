#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "Widgets/SWidget.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "AssetRegistry/AssetData.h"
#include "PreviewScene.h"
#include "GraphEditor.h"
#include "BlueprintEditor.h"
#include "Blueprint/UserWidget.h"

#include "StateMachine/EditorCommandEvents.h"

class STATEMACHINEEDITOR_API FEditor : public FBlueprintEditor
{
private:
	/** The toolbar builder associated with this editor */
	TSharedPtr<class FEditorToolbar> EditorToolbar;


public:
	/** Commands for switching between tool palettes */
	TArray< TSharedPtr<FUICommandInfo>> ToolPaletteCommands;
	FEditorCommandEvents CommandEvents;

public:
	FEditor();

	virtual ~FEditor();

	void InitEditor(
		const EToolkitMode::Type Mode, 
		const TSharedPtr< IToolkitHost >& InitToolkitHost, 
		const TArray<UBlueprint*>& InBlueprints, 
		bool bShouldOpenInDefaultsMode);

	//~ Begin FBlueprintEditor interface
	virtual void Tick(float DeltaTime) override;
	virtual void PostUndo(bool bSuccessful) override;
	virtual void PostRedo(bool bSuccessful) override;
	virtual void Compile() override;
	//~ End FBlueprintEditor interface


	//~ Begin FAssetEditorToolkit Interface
	virtual bool OnRequestClose(EAssetEditorCloseReason InCloseReason) override;
	// End of FAssetEditorToolkit 

	//~ Begin FGCObjectInterface interface
	virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
	//~ End FGCObjectInterface interface

	//~ Begin IToolkit interface
	virtual FName GetToolkitContextFName() const override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual void InitToolMenuContext(FToolMenuContext& MenuContext) override;
	virtual void OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit) override;
	virtual void OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit) override;
	//~ End IToolkit interface

	//~ Begin FBlueprintEditor interface
	virtual void RegisterApplicationModes(const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode, bool bNewlyCreated = false) override;
	virtual FGraphAppearanceInfo GetGraphAppearance(class UEdGraph* InGraph) const override;
	//virtual TSubclassOf<UEdGraphSchema> GetDefaultSchemaClass() const override;
	//~ End FBlueprintEditor interface

	void CreateEditorModeManager() override;

	class UStateMachineBlueprint* GetStateMachineBlueprintObj();

	TSharedPtr<class FEditorToolbar> GetWidgetToolbarBuilder() { return EditorToolbar; }

protected:
	virtual void InitalizeExtenders() override;
	
private:
	TSharedPtr<FExtender> CreateMenuExtender();
};