#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Utils/TimeGatedBool.h"
#include "Utils/SetGatedBool.h"
#include "Utils/NAryGate.h"
#include "StateMachine/StateMachine.h"
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

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static bool Contains(const FSetGatedBool& Input, UObject* Obj);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	static void GateObj(UPARAM(ref) FSetGatedBool& Input, UObject* Obj);

	/* 
	 * Rotates the Base angle to Goal angle by Delta Amount. If the difference between 
	 * Goal and Base is less than delta Goal will be returned. Undefined behaviour for negative
	 * Deltas.
	 */
	UFUNCTION(BlueprintCallable, Category = "Utility")
	static float RotateAngleTo(float Base, float Goal, float Delta);

	UFUNCTION(BlueprintCallable, Category = "RPG", meta = (ExpandEnumAsExecs = "Result", DeterminesOutputType = "SClass"))
	UObject* GetOwnerAs(UActorComponent* Component, TSubclassOf<AActor> SClass, ESearchResult& Result);

	UFUNCTION(BlueprintCallable, Category = "UtilityStructures", 
		meta = (ExpandEnumAsExecs = "Result"))
	void ActivateNAryGate(UPARAM(ref) FNAryGate& Gate, ETriggerBranch& Result);

	UFUNCTION(BlueprintCallable, Category = "UtilityStructures",
		meta = (ExpandEnumAsExecs = "Result"))
	void DeactivateNAryGate(UPARAM(ref) FNAryGate& Gate, ETriggerBranch& Result);
};