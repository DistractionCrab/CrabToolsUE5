#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Utils/TimeGatedBool.h"
#include "UtilsLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="UtilityFunctions")
	static void ActivateTimeGatedBool(UPARAM(ref) FTimeGatedBool& input);

	UFUNCTION(Blueprintpure, Category="UtilityFunctions",
		meta=(DisplayName="UnwrapGatedBool", CompactNodeTitle="->", BlueprintAutocast))
	static bool TimeGatedBoolConvert(const FTimeGatedBool& input);
};