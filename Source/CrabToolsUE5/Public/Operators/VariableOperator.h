// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VariableOperator.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class CRABTOOLSUE5_API UVariableOperator : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VariableOperators")
	int ApplyInt(int Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VariableOperators")
	int ApplyFloat(float Value);

	virtual int ApplyInt_Implementation(int Value) { return Value; }
	virtual int ApplyFloat_Implementation(float Value) { return Value; }
};
