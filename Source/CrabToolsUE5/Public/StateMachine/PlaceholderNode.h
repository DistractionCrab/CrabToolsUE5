// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateMachine.h"
#include "PlaceholderNode.generated.h"

/**
 * State Machine Node used as a mapped placeholder to be replaced. Has no other functionality.
 */
UCLASS(Blueprintable)
class CRABTOOLSUE5_API UPlaceholderNode : public UStateNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category="ProcStateMachine")
	FName SlotName;

public:

	UFUNCTION(BlueprintCallable, Category="ProcStateMachine")
	FORCEINLINE FName GetSlotName() { return this->SlotName; }

	virtual UStateNode* Substitute(FName SlotName, UStateNode* Node) override;
};
