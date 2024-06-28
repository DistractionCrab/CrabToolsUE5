#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "HierarchyNode.generated.h"

/**
 * State Machine node used that is controlled by a StateMachine.
 */
UCLASS(Category = "StateMachine")
class CRABTOOLSUE5_API UHierarchyNode : public UStateNode
{
	GENERATED_BODY()
	
	//UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (AllowPrivateAccess = "true"))
	//TSubclassOf<UStateMachine> MachineClass;

	UPROPERTY(VisibleAnywhere, Category = "ProcStateMachine", 
		meta = (AllowPrivateAccess = "true", ShowInnerProperties, ShowOnlyInnerProperties))
	TObjectPtr<UStateMachine> SubMachine;

	/* Name of the submachine to be placed in this node. */
	UPROPERTY(EditAnywhere, Category="StateMachine",
		meta=(GetOptions="GetMachineOptions(SlotName)"))
	FName SlotName;

	/* Map of SubMachine states and events to be emitted to the SM of this node. */
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", 
		meta = (AllowPrivateAccess = "true", 
			GetKeyOptions="GetSubMachineStateOptions(SlotName)"))
	TMap<FName, FName> ExitStates;

	/* Whether or not the submachine should be reset when this node is entered. */
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess = "true"))
	bool ResetOnEnter = true;

	//UPROPERTY(EditAnywhere, Instanced, Category = "StateMachine", meta = (AllowPrivateAccess = "true"))
	//TMap<FName, UStateNode*> SubstituteNodes;

	/*
	 * The event to pass to the submachine when entering. Useful for when ResetOnEnter is false, but
	 * work needs to be continued. Specifically, if ResetOnEnter is false, and nothing is done to transition 
	 * the state, on the next tick or event the exist state will be detected again.
	 */
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (AllowPrivateAccess = "true"))
	FName EnterEventName = "HIERARCHY_REENTER";

public:
	virtual void Initialize_Implementation() override;
	virtual void Event_Implementation(FName EName) override;
	virtual void EventWithData_Implementation(FName EName, UObject* Data) override;
	virtual void Enter_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;
	virtual void Exit_Implementation() override;
	//virtual UStateNode* Substitute(FName SlotName, UStateNode* Node) override;

	void PerformExit();

	UFUNCTION()
	TArray<FString> GetMachineOptions(FName Value) const;
};
