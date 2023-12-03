#pragma once

#include "CoreMinimal.h"
#include "Enums.generated.h"

UENUM(BlueprintType)
enum class ESearchResult : uint8 {
	Found          UMETA(DisplayName = "Found"),
	NotFound       UMETA(DisplayName = "Missing"),
};