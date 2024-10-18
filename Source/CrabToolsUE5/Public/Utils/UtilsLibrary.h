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
	 * 
	 * returns the result angle in degrees.
	 */
	UFUNCTION(BlueprintCallable, Category = "Utility")
	static float RotateAngleTo(float Base, float Goal, float Delta);

	/* 
	 * Gets the remaining amount of rotation between an angle, so that adding the delta
	 * to Base will Be goal if it's close enough. Otherwise, it'll reqturn +/- Delta
	 * depending on the closer direction.
	 * 
	 * Returns the Delta in degrees.
	*/
	UFUNCTION(BlueprintCallable, Category = "Utility")
	static float RotateAngleToDelta(float Base, float Goal, float Delta, bool& Complete);

	UFUNCTION(BlueprintCallable, Category = "RPG", meta = (ExpandEnumAsExecs = "Result", DeterminesOutputType = "SClass"))
	UObject* GetOwnerAs(UActorComponent* Component, TSubclassOf<AActor> SClass, ESearchResult& Result);

	UFUNCTION(BlueprintCallable, Category = "UtilityStructures", 
		meta = (ExpandEnumAsExecs = "Result"))
	void ActivateNAryGate(UPARAM(ref) FNAryGate& Gate, ETriggerBranch& Result);

	UFUNCTION(BlueprintCallable, Category = "UtilityStructures",
		meta = (ExpandEnumAsExecs = "Result"))
	void DeactivateNAryGate(UPARAM(ref) FNAryGate& Gate, ETriggerBranch& Result);
};

namespace UtilsFunctions
{
	template<class T> T* GetOuterAs(const UObject* Obj)
	{
		UObject* Outer = Obj->GetOuter();

		while (Outer)
		{
			if (auto Casted = Cast<T>(Outer))
			{
				return Casted;
			}
			else
			{
				Outer = Outer->GetOuter();
			}
		}

		return nullptr;
	}

	template<class T> bool SetEquals(TSet<T>& A, TSet<T>& B)
	{
		
		for (auto V1 : A)
		{
			if (!B.Contains(V1)) { return false; }
		}

		for (auto V1 : B)
		{
			if (!A.Contains(V1)) { return false; }
		}

		return true;
	}
}
