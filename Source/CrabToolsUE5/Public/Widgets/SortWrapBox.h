// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WrapBox.h"
#include "SortWrapBox.generated.h"

UCLASS()
class USortWrapBox : public UWrapBox
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintCallable, Category="Widget")
	void SortChildren();
};
