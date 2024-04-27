#include "StateMachine/TabFactory/GraphTabFactory.h"
#include "Tools/ToolCompatible.h"
#include "StateMachine/Widgets/GraphView.h"
#include "Toolkits/AssetEditorModeUILayer.h"

#define LOCTEXT_NAMESPACE "PSM"

const FName FGraphTabFactory::ToolPaletteTabID = UAssetEditorUISubsystem::TopLeftTabID;
const FName FGraphTabFactory::TabID(TEXT("GraphView"));

FGraphTabFactory::FGraphTabFactory(TSharedPtr<class FEditor> InBlueprintEditor)
: FWorkflowTabFactory(TabID, InBlueprintEditor),
	BlueprintEditor(InBlueprintEditor)
{
	TabLabel = LOCTEXT("GraphTabLabel", "Machine Graph");
	//TabIcon = FSlateIcon(FUMGStyle::GetStyleSetName(), "Graph.TabIcon");

	bIsSingleton = true;

	ViewMenuDescription = LOCTEXT("GraphView_ViewMenu_Desc", "Machine Graph");
	ViewMenuTooltip = LOCTEXT("GraphView_ViewMenu_ToolTip", "Show the Machine Graph");
}

TSharedRef<SWidget> FGraphTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedRef<TToolCompatibleMixin<SGraphView>> GraphView = 
		SNew(TToolCompatibleMixin<SGraphView>, BlueprintEditor.Pin())
		.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("Graph")));
	GraphView->SetParentToolkit(BlueprintEditor.Pin());

	// @TODO: Ideally this is done by SetParentToolkit, but can't seem to do that due to template issues with SharedThis.
	GraphView->GetWidgetModeManger()->SetManagedWidget(GraphView);
	
	return GraphView;
}

#undef LOCTEXT_NAMESPACE 