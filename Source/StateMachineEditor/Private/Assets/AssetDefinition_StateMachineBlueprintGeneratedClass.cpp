#include "Assets/AssetDefinition_StateMachineBlueprintGeneratedClass.h"

#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "Misc/MessageDialog.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UAssetDefinition_StateMachineBlueprintGeneratedClass::UAssetDefinition_StateMachineBlueprintGeneratedClass() = default;

UAssetDefinition_StateMachineBlueprintGeneratedClass::~UAssetDefinition_StateMachineBlueprintGeneratedClass() = default;

FText UAssetDefinition_StateMachineBlueprintGeneratedClass::GetAssetDisplayName() const
{
	return LOCTEXT("AssetTypeActions_StateMachineBlueprintGeneratedClass", "StateMachine Blueprint Generated Class");
}

FLinearColor UAssetDefinition_StateMachineBlueprintGeneratedClass::GetAssetColor() const
{
	return FLinearColor(FColor(44, 89, 180));
}

TSoftClassPtr<> UAssetDefinition_StateMachineBlueprintGeneratedClass::GetAssetClass() const
{
	return UStateMachineBlueprintGeneratedClass::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_StateMachineBlueprintGeneratedClass::GetAssetCategories() const
{
	static const TArray<FAssetCategoryPath, TFixedAllocator<1>> Categories = { EAssetCategoryPaths::Misc };
	return Categories;
}

#undef LOCTEXT_NAMESPACE