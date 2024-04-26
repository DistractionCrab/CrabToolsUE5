#pragma once

#include "StateMachine/ProcStateMachineBlueprintGeneratedClass.h"
#include "AssetDefinitionDefault.h"

#include "AssetDefinition_ProcStateMachineBlueprintGeneratedClass.generated.h"

UCLASS()
class UAssetDefinition_ProcStateMachineBlueprintGeneratedClass : public UAssetDefinitionDefault
{
	GENERATED_BODY()

public:
	UAssetDefinition_ProcStateMachineBlueprintGeneratedClass();
	virtual ~UAssetDefinition_ProcStateMachineBlueprintGeneratedClass() override;
	
	virtual FText GetAssetDisplayName() const override;
	virtual FLinearColor GetAssetColor() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
};