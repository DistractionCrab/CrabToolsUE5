// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Delegates/Delegate.h"
#include "ProcStateMachine.generated.h"

class UStateNode;
class UProcStateMachine;
class UNodeTransition;

USTRUCT(BlueprintType)
struct  FTransitionData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine")
	TSubclassOf<UNodeTransition> TranstionClass;
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine")
	UNodeTransition* ProcTransition;
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine")
	FName Destination;
};

USTRUCT(BlueprintType)
struct  FStateData
{
	GENERATED_USTRUCT_BODY()
public:
	//UPROPERTY(EditAnywhere, Category = "ProcStateMachine")
	//TSubclassOf<UStateNode> NodeClass;
	UPROPERTY(EditAnywhere, Instanced, Category = "ProcStateMachine")
	UStateNode* Node;
	// Map from Event Name to StateName
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine")
	TMap<FName, FName> EventTransitions;
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine")
	FTransitionData Transitions;
};

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class CRABTOOLSUE5_API UStateNode : public UObject
{
	GENERATED_BODY()

	UProcStateMachine* Owner;

public:
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine")
	bool bNeedsTick = false;

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void Initialize(UProcStateMachine* POwner);
	virtual void Initialize_Implementation(UProcStateMachine* POwner);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void Enter();
	virtual void Enter_Implementation() {}
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void Tick(float DeltaTime);
	virtual void Tick_Implementation(float DeltaTime) {}
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void Exit();
	virtual void Exit_Implementation() {}

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	void GoTo(FName State);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void Event(FName EName);
	virtual void Event_Implementation(FName EName);

	AActor* GetOwner();
};



DECLARE_DYNAMIC_DELEGATE_TwoParams(FStateChangeDispatcher, FName, From, FName, To);
/**
 *
 */
UCLASS(Blueprintable)
class CRABTOOLSUE5_API UProcStateMachine : public UObject
{
	GENERATED_BODY()

	// Internal Structure used for keeping track of state transitions.
	struct Transition {
	private:
		int ID = 0;

	public:
		int EnterTransition() {
			this->ID += 1;
			return ID;
		}

		bool Valid(int OID) {
			return OID == this->ID;
		}
	} TRANSITION;

	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (AllowPrivateAccess = "true"))
	FName StartState;
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (AllowPrivateAccess = "true"))
	TMap<FName, FStateData> Graph;
	FName CurrentStateName;
	AActor* Owner;


public:
	UPROPERTY(VisibleAnywhere, Category = "ProcStateMachine")
	TArray<FStateChangeDispatcher> StateChangeEvents;
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void Initialize(AActor* POwner);
	virtual void Initialize_Implementation(AActor* POwner);
	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	AActor* GetOwner();
	/* 
	* Resets the state machine to its start state. Does not trigger StateChanged Events.
	*/
	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	void Reset();
	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	void Event(FName EName);

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	bool NeedsTick();

	/* 
	* Tick function to be called regularly. This is managed by the owner object.
	*/
	void Tick(float DeltaTime);
	/**
	 * This function should not be called except by UStateNode Implementations or unless you
	 * specifically require a state to be forced regardless of the context.
	 */
	void UpdateState(FName Name);
	void ListenForChange(const FStateChangeDispatcher& obs);

	FORCEINLINE FStateData& GetCurrentState() { return this->Graph[this->CurrentStateName]; }
};
