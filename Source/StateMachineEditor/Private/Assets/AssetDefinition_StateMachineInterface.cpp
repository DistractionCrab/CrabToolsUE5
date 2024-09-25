#include "Assets/AssetDefinition_StateMachineInterface.h"

#include "StateMachine/Editor.h"
#include "StateMachine/StateMachineInterface.h"
#include "Misc/MessageDialog.h"
#include "SBlueprintDiff.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UAssetDefinition_StateMachineInterface::UAssetDefinition_StateMachineInterface() = default;

UAssetDefinition_StateMachineInterface::~UAssetDefinition_StateMachineInterface() = default;

FText UAssetDefinition_StateMachineInterface::GetAssetDisplayName() const
{
	return LOCTEXT("AssetTypeActions_StateMachineInterface", "State Machine Interface");
}

FLinearColor UAssetDefinition_StateMachineInterface::GetAssetColor() const
{
	return FLinearColor(FColor(3, 252, 248));
}

TSoftClassPtr<> UAssetDefinition_StateMachineInterface::GetAssetClass() const
{
	return UStateMachineInterface::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_StateMachineInterface::GetAssetCategories() const
{
	static const TArray<FAssetCategoryPath, TFixedAllocator<1>> Categories = { EAssetCategoryPaths::AI };
	return Categories;
}


#undef LOCTEXT_NAMESPACE