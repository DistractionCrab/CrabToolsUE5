#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

class FProcStateMachineAssetActions : public FAssetTypeActions_Base
{
public:
	UClass* GetSupportedClass() const override;
	FText GetName() const override;
	FColor GetTypeColor() const override;
	uint32 GetCategories() override;
};