#include "Assets/AssetDefinition_ProcStateMachineBlueprintGeneratedClass.h"

#include "StateMachine/ProcStateMachineBlueprintGeneratedClass.h"
#include "Misc/MessageDialog.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

UAssetDefinition_ProcStateMachineBlueprintGeneratedClass::UAssetDefinition_ProcStateMachineBlueprintGeneratedClass() = default;

UAssetDefinition_ProcStateMachineBlueprintGeneratedClass::~UAssetDefinition_ProcStateMachineBlueprintGeneratedClass() = default;

FText UAssetDefinition_ProcStateMachineBlueprintGeneratedClass::GetAssetDisplayName() const
{
	return LOCTEXT("AssetTypeActions_ProcStateMachineBlueprintGeneratedClass", "ProcStateMachine Blueprint Generated Class");
}

FLinearColor UAssetDefinition_ProcStateMachineBlueprintGeneratedClass::GetAssetColor() const
{
	return FLinearColor(FColor(44, 89, 180));
}

TSoftClassPtr<> UAssetDefinition_ProcStateMachineBlueprintGeneratedClass::GetAssetClass() const
{
	return UProcStateMachineBlueprintGeneratedClass::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UAssetDefinition_ProcStateMachineBlueprintGeneratedClass::GetAssetCategories() const
{
	static const TArray<FAssetCategoryPath, TFixedAllocator<1>> Categories = { EAssetCategoryPaths::Misc };
	return Categories;
}

#undef LOCTEXT_NAMESPACE