// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/UniformGridPanel.h"
#include "SortUniformGridPanel.generated.h"

UCLASS()
class USortUniformGridPanel : public UUniformGridPanel
{
	GENERATED_BODY()	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Child Layout", meta=(AllowPrivateAccess=true, ClampMin=1, UIMin=1))
	int Width;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Child Layout", meta = (AllowPrivateAccess = true, ClampMin = 1, UIMin = 1))
	int Height;

public:
	UFUNCTION(BlueprintCallable, Category="Widget")
	void SortChildren();
};
