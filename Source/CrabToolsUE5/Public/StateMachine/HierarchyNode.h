// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProcStateMachine.h"
#include "HierarchyNode.generated.h"

/**
 * State Machine node used that is controlled by a StateMachine.
 */
UCLASS()
class CRABTOOLSUE5_API UHierarchyNode : public UStateNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UProcStateMachine> MachineClass;
	UProcStateMachine* Machine;

public:
	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	virtual void Initialize(UProcStateMachine* POwner) override;
};
