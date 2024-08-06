#pragma once

#include "CoreMinimal.h"
#include "EventSet.generated.h"

USTRUCT(BlueprintType)
struct FEventSetRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "EventSet",
		meta = (MultiLine = true))
	FText Description;
};
