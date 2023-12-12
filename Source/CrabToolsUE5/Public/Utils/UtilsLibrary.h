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
	UFUNCTION(BlueprintCallable, Category="Utility")
	static void ActivateTimeGatedBool(UPARAM(ref) FTimeGatedBool& input);

	UFUNCTION(Blueprintpure, Category="Utility",
		meta=(DisplayName="UnwrapGatedBool", CompactNodeTitle="->", BlueprintAutocast))
	static bool TimeGatedBoolConvert(const FTimeGatedBool& input);

	/* 
	 * Rotates the Base angle to Goal angle by Delta Amount. If the difference between 
	 * Goal and Base is less than delta Goal will be returned. Undefined behaviour for negative
	 * Deltas.
	 */
	UFUNCTION(BlueprintCallable, Category = "Utility")
	static float RotateAngleTo(float Base, float Goal, float Delta);
};