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
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (AllowPrivateAccess = "true"))
	UProcStateMachine* SubMachine;

	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (AllowPrivateAccess = "true"))
	TMap<FName, FName> ExitStates;

	/* Whether or not the submachine should be reset when this node is entered. */
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (AllowPrivateAccess = "true"))
	bool ResetOnEnter = true;

	/*
	 * The event to pass to the submachine when entering. Useful for when ResetOnEnter is false, but
	 * work needs to be continued. Specifically, if ResetOnEnter is false, and nothing is done to transition 
	 * the state, on the next tick or event the exist state will be detected again.
	 */
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (AllowPrivateAccess = "true"))
	FName EnterEventName = "HIERARCHY_REENTER";
public:
	virtual void Initialize_Implementation(UProcStateMachine* POwner) override;
	virtual void Event_Implementation(FName EName) override;
	virtual void Enter_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;
	virtual void Exit_Implementation() override;

	void PerformExit();
};
