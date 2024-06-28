#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "EventMapNode.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FEventHandler, FName, EventName);

/**
 * State Machine Node that is a composite of other nodes.
 */
UCLASS(Blueprintable, Category = "StateMachine")
class CRABTOOLSUE5_API UEventMapNode : public UStateNode
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (GetValueOptions="HandlerNameOptions"))
	TMap<FName, FName> EventMap;
	TMap<FName, FEventHandler> EventMapInternal;

public:
	virtual void Event_Implementation(FName Event) override;
	virtual void Initialize_Implementation() override;

	UFUNCTION()
	TArray<FString> HandlerNameOptions();
};
