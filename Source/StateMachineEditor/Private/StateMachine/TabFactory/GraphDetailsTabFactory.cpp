// Copyright Epic Games, Inc. All Rights Reserved.

#include "StateMachine/TabFactory/GraphDetailsTabFactory.h"

#if WITH_EDITOR
	#include "Styling/AppStyle.h"
#endif // WITH_EDITOR

#include "StateMachine/Widgets/GraphDetailsView.h"

#define LOCTEXT_NAMESPACE "PSM"

const FName FGraphDetailsTabFactory::TabID(TEXT("GraphDetails"));

FGraphDetailsTabFactory::FGraphDetailsTabFactory(
	TSharedPtr<class FEditor> InBlueprintEditor)
: FWorkflowTabFactory(TabID, InBlueprintEditor), 
	BlueprintEditor(InBlueprintEditor)
{
	TabLabel = LOCTEXT("GraphDetails_TabLabel", "Graph Details");
	TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.GraphDetails");

	bIsSingleton = true;

	ViewMenuDescription = LOCTEXT("GraphDetails_ViewMenu_Desc", "Graph Details");
	ViewMenuTooltip = LOCTEXT("GraphDetails_ViewMenu_ToolTip", "Show the Details");
}

TSharedRef<SWidget> FGraphDetailsTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<FEditor> BlueprintEditorPtr = StaticCastSharedPtr<FEditor>(BlueprintEditor.Pin());

	return SNew(SGraphDetailsView, BlueprintEditorPtr)
		.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("Details")));
}

#undef LOCTEXT_NAMESPACE 