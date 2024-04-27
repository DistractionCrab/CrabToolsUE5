#pragma once

#include "StateMachine/StateMachineBlueprintGeneratedClass.h"
#include "AssetDefinitionDefault.h"

#include "AssetDefinition_StateMachineBlueprintGeneratedClass.generated.h"

UCLASS()
class UAssetDefinition_StateMachineBlueprintGeneratedClass : public UAssetDefinitionDefault
{
	GENERATED_BODY()

public:
	UAssetDefinition_StateMachineBlueprintGeneratedClass();
	virtual ~UAssetDefinition_StateMachineBlueprintGeneratedClass() override;
	
	virtual FText GetAssetDisplayName() const override;
	virtual FLinearColor GetAssetColor() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
};