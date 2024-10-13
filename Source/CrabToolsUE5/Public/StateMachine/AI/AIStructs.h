#pragma once

#include "AIStructs.generated.h"

USTRUCT(BlueprintType)
struct FMoveToData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	FVector DestinationLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "StateMachine|AI")
	TObjectPtr<AActor> DestinationActor;

	FMoveToData() : DestinationLocation(0, 0, 0) {}
};