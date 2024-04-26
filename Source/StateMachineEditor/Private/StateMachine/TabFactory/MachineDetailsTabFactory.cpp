// Copyright Epic Games, Inc. All Rights Reserved.

#include "StateMachine/TabFactory/MachineDetailsTabFactory.h"

#if WITH_EDITOR
	#include "Styling/AppStyle.h"
#endif // WITH_EDITOR

#include "StateMachine/Widgets/MachineDetailsView.h"

#define LOCTEXT_NAMESPACE "PSM"

const FName FMachineDetailsTabFactory::TabID(TEXT("MachineDetails"));

FMachineDetailsTabFactory::FMachineDetailsTabFactory(
	TSharedPtr<class FProcStateMachineBlueprintEditor> InBlueprintEditor)
: FWorkflowTabFactory(TabID, InBlueprintEditor), 
	BlueprintEditor(InBlueprintEditor)
{
	TabLabel = LOCTEXT("MachineDetails_TabLabel", "Machine Details");
	TabIcon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.MachineDetails");

	bIsSingleton = true;

	ViewMenuDescription = LOCTEXT("MachineDetails_ViewMenu_Desc", "Machine Details");
	ViewMenuTooltip = LOCTEXT("MachineDetails_ViewMenu_ToolTip", "Show the Details");
}

TSharedRef<SWidget> FMachineDetailsTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<FProcStateMachineBlueprintEditor> BlueprintEditorPtr = StaticCastSharedPtr<FProcStateMachineBlueprintEditor>(BlueprintEditor.Pin());

	return SNew(SMachineDetailsView, BlueprintEditorPtr)
		.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("Details")));
}

#undef LOCTEXT_NAMESPACE 