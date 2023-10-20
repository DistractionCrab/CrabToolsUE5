// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProcStateMachine.h"
#include "NodeTransition.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class CRABTOOLSUE5_API UNodeTransition : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, Category = "ProcStateMachine")
	bool CheckCondition(UProcStateMachine* Machine);
	virtual bool CheckCondition_Implementation(UProcStateMachine* Machine);
};
