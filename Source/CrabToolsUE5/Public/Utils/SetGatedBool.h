#pragma once

#include "CoreMinimal.h"
#include "SetGatedBool.generated.h"


USTRUCT(BlueprintType)
struct FSetGatedBool
{
	GENERATED_USTRUCT_BODY()

private:

	UPROPERTY(BlueprintReadOnly, Category="UtilityStructures", meta=(AllowPrivateAccess=true))
	TSet<UObject*> Objects;


public:
	bool ContainsObj(UObject* Obj) const;
	void AddObj(UObject* Obj);
};