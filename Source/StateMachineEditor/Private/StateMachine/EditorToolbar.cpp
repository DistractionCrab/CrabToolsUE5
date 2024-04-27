#include "StateMachine/EditorToolbar.h"
#include "Types/ISlateMetaData.h"
#include "Misc/Attribute.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSpacer.h"

#if WITH_EDITOR
	#include "Styling/AppStyle.h"
#endif // WITH_EDITOR

#include "Widgets/SToolTip.h"
#include "IDocumentation.h"
#include "BlueprintEditor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ToolMenus.h"
#include "BlueprintEditorContext.h"

#include "StateMachine/Editor.h"
#include "WorkflowOrientedApp/SModeWidget.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "StateMachine/BlueprintModes/BlueprintApplicationMode.h"
#include "StateMachine/BlueprintModes/GraphApplicationMode.h"

#define LOCTEXT_NAMESPACE "PSM"

class SBlueprintModeSeparator : public SBorder
{
public:
	SLATE_BEGIN_ARGS(SBlueprintModeSeparator) {}
	SLATE_END_ARGS()

		void Construct(const FArguments& InArg)
	{
		SBorder::Construct(
			SBorder::FArguments()
			.BorderImage(FAppStyle::GetBrush("BlueprintEditor.PipelineSeparator"))
			.Padding(0.0f)
			);
	}

	// SWidget interface
	virtual FVector2D ComputeDesiredSize(float) const override
	{
		const float Height = 20.0f;
		const float Thickness = 16.0f;
		return FVector2D(Thickness, Height);
	}
	// End of SWidget interface
};

FEditorToolbar::FEditorToolbar(TSharedPtr<FEditor>& InEditor)
: MyEditor(InEditor)
{
}

void FEditorToolbar::AddEditorModesToolbar(TSharedPtr<FExtender> Extender)
{
	
	TSharedPtr<FEditor> BlueprintEditorPtr = MyEditor.Pin();

	Extender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		BlueprintEditorPtr->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateSP(this, 
			&FEditorToolbar::FillEditorModesToolbar));
}

void FEditorToolbar::FillEditorModesToolbar(FToolBarBuilder& ToolbarBuilder)
{
	TSharedPtr<FEditor> BlueprintEditorPtr = MyEditor.Pin();
	UBlueprint* BlueprintObj = BlueprintEditorPtr->GetBlueprintObj();

	if( !BlueprintObj ||
		(!FBlueprintEditorUtils::IsLevelScriptBlueprint(BlueprintObj) 
		&& !FBlueprintEditorUtils::IsInterfaceBlueprint(BlueprintObj)
		&& !BlueprintObj->bIsNewlyCreated)
		)
	{
		TAttribute<FName> GetActiveMode(
			BlueprintEditorPtr.ToSharedRef(), 
			&FBlueprintEditor::GetCurrentMode);
		FOnModeChangeRequested SetActiveMode = FOnModeChangeRequested::CreateSP(
			BlueprintEditorPtr.ToSharedRef(), 
			&FBlueprintEditor::SetCurrentMode);

		// Left side padding
		BlueprintEditorPtr->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));

		BlueprintEditorPtr->AddToolbarWidget(
			SNew(SModeWidget, 
				FGraphApplicationMode::GetLocalizedMode(
					FGraphApplicationMode::ModeName), 
				FGraphApplicationMode::ModeName)
			.OnGetActiveMode(GetActiveMode)
			.OnSetActiveMode(SetActiveMode)
			.ToolTip(IDocumentation::Get()->CreateToolTip(
				LOCTEXT("DesignerModeButtonTooltip", "Switch to Graph Mode"),
				nullptr,
				TEXT("Shared/Editors/BlueprintEditor"),
				TEXT("GraphMode")))
			.IconImage(FAppStyle::GetBrush("UMGEditor.SwitchToDesigner"))
			.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("GraphMode")))
		);

		BlueprintEditorPtr->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(10.0f, 1.0f)));

		BlueprintEditorPtr->AddToolbarWidget(
			SNew(
				SModeWidget, 
				FBlueprintApplicationMode::GetLocalizedMode(
					FBlueprintApplicationMode::ModeName), 
				FBlueprintApplicationMode::ModeName)
			.OnGetActiveMode(GetActiveMode)
			.OnSetActiveMode(SetActiveMode)
			.CanBeSelected(BlueprintEditorPtr.Get(), &FBlueprintEditor::IsEditingSingleBlueprint)
			.ToolTip(IDocumentation::Get()->CreateToolTip(
				LOCTEXT("GraphModeButtonTooltip", "Switch to Blueprint Editing Mode"),
				nullptr,
				TEXT("Shared/Editors/BlueprintEditor"),
				TEXT("BlueprintMode")))
			.IconImage(FAppStyle::GetBrush("FullBlueprintEditor.SwitchToScriptingMode"))
			.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("BlueprintMode")))
		);
		
		// Right side padding
		BlueprintEditorPtr->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(10.0f, 1.0f)));

		/*
		if (FProcStateMachineBlueprintApplicationModes::IsPreviewModeEnabled())
		{
			BlueprintEditorPtr->AddToolbarWidget(
				SNew(SModeWidget, FProcStateMachineBlueprintApplicationModes::GetLocalizedMode(FProcStateMachineBlueprintApplicationModes::PreviewMode), FProcStateMachineBlueprintApplicationModes::PreviewMode)
				.OnGetActiveMode(GetActiveMode)
				.OnSetActiveMode(SetActiveMode)
				.CanBeSelected(BlueprintEditorPtr.Get(), &FBlueprintEditor::IsEditingSingleBlueprint)
				.ToolTip(IDocumentation::Get()->CreateToolTip(
					LOCTEXT("PreviewModeButtonTooltip", "Switch to Preview Mode"),
					nullptr,
					TEXT("Shared/Editors/BlueprintEditor"),
					TEXT("DebugMode")))
				.IconImage(FAppStyle::GetBrush("BlueprintDebugger.TabIcon"))
				.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("PreviewMode")))
			);
		
			// Right side padding
			BlueprintEditorPtr->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(10.0f, 1.0f)));
		}
		*/
	}
}

void FEditorToolbar::AddWidgetReflector(UToolMenu* InMenu)
{
	FToolMenuSection& Section = InMenu->AddSection("StateMachineTools");
	Section.InsertPosition = FToolMenuInsert("SourceControl", EToolMenuInsertType::After);

	Section.AddEntry(FToolMenuEntry::InitToolBarButton(
		"OpenStateMachineReflector",
		FUIAction(
			FExecuteAction::CreateLambda([=] { FGlobalTabmanager::Get()->TryInvokeTab(FTabId("WidgetReflector")); }),
			FCanExecuteAction()
		)
		, LOCTEXT("OpenStateMachineReflector", "Widget Reflector")
		, LOCTEXT("OpenWidgetStateMachineToolTip", "Opens the State Machine Reflector, a handy tool for diagnosing problems with live state machines.")
		, FSlateIcon(FCoreStyle::Get().GetStyleSetName(), "WidgetReflector.Icon")
	));
}

void FEditorToolbar::AddToolPalettes(UToolMenu* InMenu)
{
	// @TODO: For now we only support one tool palette, switch this to a dropdown when we support multiple tool palettes.
	for (TSharedPtr<FUICommandInfo>& Command : MyEditor.Pin()->ToolPaletteCommands)
	{
		FToolMenuSection& Section = InMenu->FindOrAddSection("UMGToolPalette");
		Section.AddDynamicEntry(Command->GetCommandName(), FNewToolMenuSectionDelegate::CreateLambda([this, Command](FToolMenuSection& InSection)
			{
				InSection.AddEntry(FToolMenuEntry::InitToolBarButton(Command));
			}));
	}
}

#undef LOCTEXT_NAMESPACE