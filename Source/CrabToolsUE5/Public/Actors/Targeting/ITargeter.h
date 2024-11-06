#pragma once

#include "CoreMinimal.h"
#include "ITargeter.generated.h"

UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UTargeterInterface : public UInterface
{
	GENERATED_BODY()
};

class ITargeterInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Targeting|LineTrace")
	FVector GetEndPoint() const;
	virtual FVector GetEndPoint_Implementation() const { return FVector::Zero(); }
};