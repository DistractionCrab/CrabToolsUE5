#include "Assets/AssetDefinition_ProcStateMachineBlueprint.h"

#include "StateMachine/ProcStateMachineBlueprintEditor.h"
#include "StateMachine/ProcStateMachineBlueprint.h"
#include "Misc/MessageDialog.h"
#include "SBlueprintDiff.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UAssetDefinition_ProcStateMachineBlueprint::UAssetDefinition_ProcStateMachineBlueprint() = default;

UAssetDefinition_ProcStateMachineBlueprint::~UAssetDefinition_ProcStateMachineBlueprint() = default;

FText UAssetDefinition_ProcStateMachineBlueprint::GetAssetDisplayName() const
{
	return LOCTEXT("AssetTypeActions_ProcStateMachineBlueprint", "ProcStateMachine Blueprint");
}

FLinearColor UAssetDefinition_ProcStateMachineBlueprint::GetAssetColor() const
{
	return FLinearColor(FColor(44, 89, 180));
}

TSoftClassPtr<> UAssetDefinition_ProcStateMachineBlueprint::GetAssetClass() const
{
	return UProcStateMachineBlueprint::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_ProcStateMachineBlueprint::GetAssetCategories() const
{
	static const TArray<FAssetCategoryPath, TFixedAllocator<1>> Categories = { EAssetCategoryPaths::Misc };
	return Categories;
}

EAssetCommandResult UAssetDefinition_ProcStateMachineBlueprint::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
	EToolkitMode::Type Mode = OpenArgs.GetToolkitMode();

	EAssetCommandResult Result = EAssetCommandResult::Unhandled;

	for (UBlueprint* Blueprint : OpenArgs.LoadObjects<UBlueprint>())
	{
		if (Blueprint && Blueprint->SkeletonGeneratedClass && Blueprint->GeneratedClass)
		{
			TSharedRef<FProcStateMachineBlueprintEditor> NewBlueprintEditor(new FProcStateMachineBlueprintEditor);

			const bool bShouldOpenInDefaultsMode = false;
			TArray<UBlueprint*> Blueprints;
			Blueprints.Add(Blueprint);

			NewBlueprintEditor->InitProcStateMachineBlueprintEditor(Mode, OpenArgs.ToolkitHost, Blueprints, bShouldOpenInDefaultsMode);
		}
		else
		{
			FMessageDialog::Open( EAppMsgType::Ok, LOCTEXT("FailedToLoadProcStateMachineBlueprint", "Widget Blueprint could not be loaded because it derives from an invalid class.\nCheck to make sure the parent class for this blueprint hasn't been removed!"));
		}

		Result = EAssetCommandResult::Handled;
	}

	return Result;
}

EAssetCommandResult UAssetDefinition_ProcStateMachineBlueprint::PerformAssetDiff(const FAssetDiffArgs& DiffArgs) const
{
	const UBlueprint* OldBlueprint = Cast<UBlueprint>(DiffArgs.OldAsset);
	const UBlueprint* NewBlueprint = Cast<UBlueprint>(DiffArgs.NewAsset);
	UClass* AssetClass = GetAssetClass().Get();
	SBlueprintDiff::CreateDiffWindow(OldBlueprint, NewBlueprint, DiffArgs.OldRevision, DiffArgs.NewRevision, AssetClass);
	return EAssetCommandResult::Handled;
}

FText UAssetDefinition_ProcStateMachineBlueprint::GetAssetDescription(const FAssetData& AssetData) const
{
	FString Description = AssetData.GetTagValueRef<FString>( GET_MEMBER_NAME_CHECKED( UBlueprint, BlueprintDescription ) );
	if ( !Description.IsEmpty() )
	{
		Description.ReplaceInline( TEXT( "\\n" ), TEXT( "\n" ) );
		return FText::FromString( MoveTemp(Description) );
	}

	return FText::GetEmpty();
}

#undef LOCTEXT_NAMESPACE