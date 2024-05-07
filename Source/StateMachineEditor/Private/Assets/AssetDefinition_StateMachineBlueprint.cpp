#include "Assets/AssetDefinition_StateMachineBlueprint.h"

#include "StateMachine/Editor.h"
#include "StateMachine/StateMachineBlueprint.h"
#include "Misc/MessageDialog.h"
#include "SBlueprintDiff.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UAssetDefinition_StateMachineBlueprint::UAssetDefinition_StateMachineBlueprint() = default;

UAssetDefinition_StateMachineBlueprint::~UAssetDefinition_StateMachineBlueprint() = default;

FText UAssetDefinition_StateMachineBlueprint::GetAssetDisplayName() const
{
	return LOCTEXT("AssetTypeActions_StateMachineBlueprint", "State Machine Blueprint");
}

FLinearColor UAssetDefinition_StateMachineBlueprint::GetAssetColor() const
{
	return FLinearColor(FColor(44, 89, 180));
}

TSoftClassPtr<> UAssetDefinition_StateMachineBlueprint::GetAssetClass() const
{
	return UStateMachineBlueprint::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_StateMachineBlueprint::GetAssetCategories() const
{
	static const TArray<FAssetCategoryPath, TFixedAllocator<1>> Categories = { EAssetCategoryPaths::Misc };
	return Categories;
}

EAssetCommandResult UAssetDefinition_StateMachineBlueprint::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
	EToolkitMode::Type Mode = OpenArgs.GetToolkitMode();

	EAssetCommandResult Result = EAssetCommandResult::Unhandled;

	for (UBlueprint* Blueprint : OpenArgs.LoadObjects<UBlueprint>())
	{
		if (Blueprint && Blueprint->SkeletonGeneratedClass && Blueprint->GeneratedClass)
		{
			TSharedRef<FEditor> NewBlueprintEditor(new FEditor);

			const bool bShouldOpenInDefaultsMode = false;
			TArray<UBlueprint*> Blueprints;
			Blueprints.Add(Blueprint);

			NewBlueprintEditor->InitEditor(Mode, OpenArgs.ToolkitHost, Blueprints, bShouldOpenInDefaultsMode);
		}
		else
		{
			FMessageDialog::Open( EAppMsgType::Ok, LOCTEXT("FailedToLoadStateMachineBlueprint", "Widget Blueprint could not be loaded because it derives from an invalid class.\nCheck to make sure the parent class for this blueprint hasn't been removed!"));
		}

		Result = EAssetCommandResult::Handled;
	}

	return Result;
}

EAssetCommandResult UAssetDefinition_StateMachineBlueprint::PerformAssetDiff(const FAssetDiffArgs& DiffArgs) const
{
	const UBlueprint* OldBlueprint = Cast<UBlueprint>(DiffArgs.OldAsset);
	const UBlueprint* NewBlueprint = Cast<UBlueprint>(DiffArgs.NewAsset);
	UClass* AssetClass = GetAssetClass().Get();
	SBlueprintDiff::CreateDiffWindow(OldBlueprint, NewBlueprint, DiffArgs.OldRevision, DiffArgs.NewRevision, AssetClass);
	return EAssetCommandResult::Handled;
}

FText UAssetDefinition_StateMachineBlueprint::GetAssetDescription(const FAssetData& AssetData) const
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