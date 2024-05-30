#pragma once

#include "CoreMinimal.h"
#include "NAryGate.generated.h"


/* Enum used for binary branching for gates. */
UENUM(BlueprintType)
enum class ETriggerBranch : uint8 {
	Trigger          UMETA(DisplayName = "Trigger"),
	Null        UMETA(DisplayName = "Null"),
};

USTRUCT(BlueprintType)
struct FNAryGate
{
	GENERATED_USTRUCT_BODY()

private:

	UPROPERTY(EditDefaultsOnly, Category="UtilityStructures", meta=(AllowPrivateAccess=true))
	int CountGate = 1;
	int CurrentCount = 0;

public:
	ETriggerBranch Activate();
	ETriggerBranch Deactivate();
};