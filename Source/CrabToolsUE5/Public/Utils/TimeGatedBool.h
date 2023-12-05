#pragma once

#include "CoreMinimal.h"
#include "TimeGatedBool.generated.h"


USTRUCT(BlueprintType)
struct FTimeGatedBool
{
	GENERATED_USTRUCT_BODY()

private:

	UPROPERTY(BlueprintReadOnly, Category="UtilityStructures", meta=(AllowPrivateAccess=true))
	bool Value = false;
	FTimerHandle Timer;

	void Done();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UtilityStructures")
	float Time = 0.0;


public:
	bool GetValue() const;
	void Reset();
};