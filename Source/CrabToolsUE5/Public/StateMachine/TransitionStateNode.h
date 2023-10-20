// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProcStateMachine.h"
#include "NodeTransition.h"
#include "TransitionStateNode.generated.h"

/**
 * 
 */
UCLASS()
class CRABTOOLSUE5_API UTransitionStateNode : public UStateNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (AllowPrivateAccess = "true"))
	TArray<UNodeTransition*> TransitionRules;

public:
};
