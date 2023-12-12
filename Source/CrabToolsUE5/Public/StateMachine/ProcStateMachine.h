// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Delegates/Delegate.h"
#include "Templates/UniquePtr.h"
#include "StateChangeListener.h"
#include "Utils/Enums.h"
#include "UObject/ObjectPtr.h"
#include "ProcStateMachine.generated.h"

class UStateNode;
class UProcStateMachine;
class UNodeTransition;


DECLARE_DYNAMIC_DELEGATE_TwoParams(FStateChangeDispatcher, FName, From, FName, To);
DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FTransitionDelegate);
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FTransitionDataDelegate, UObject*, Data);

USTRUCT(BlueprintType)
struct  FTransitionData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (GetOptions = "StateOptions"))
	FName Destination;
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (GetOptions = "ConditionOptions"))
	FName Condition = "TrueCondition";
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (GetOptions = "ConditionDataOptions"))
	FName DataCondition = "TrueDataCondition";
	
	FTransitionDelegate ConditionCallback;
	FTransitionDataDelegate DataConditionCallback;
};



USTRUCT(BlueprintType)
struct  FStateData
{
	GENERATED_USTRUCT_BODY()
public:
	//UPROPERTY(EditAnywhere, Category = "ProcStateMachine")
	//TSubclassOf<UStateNode> NodeClass;
	UPROPERTY(EditAnywhere, Instanced, Category = "ProcStateMachine")
	TObjectPtr<UStateNode> Node;
	// Map from Event Name to StateName
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine")
	TMap<FName, FTransitionData> Transitions;
};

USTRUCT(BlueprintType)
struct FAliasData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (GetOptions = "StateOptions"))
	TSet<FName> States;

	// Mapping of EventName -> TransitionData.
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (GetValueOptions = "StateOptions"))
	TMap<FName, FTransitionData> Transitions;

};

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, CollapseCategories)
class CRABTOOLSUE5_API UStateNode : public UObject
{
	GENERATED_BODY()

	TObjectPtr<UProcStateMachine> Owner;
	bool bActive = false;

public:

	/* Function called by Initialize_Internal. Override this to setup your init code. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void Initialize(UProcStateMachine* POwner);
	virtual void Initialize_Implementation(UProcStateMachine* POwner);

	/**
	 * Function used to ensure proper state setup happens. Only call this if you need to manually initialize a
	 * state machine.
	 */
	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	void Initialize_Internal(UProcStateMachine* POwner);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void Event(FName EName);
	void Event_Internal(FName EName);
	virtual void Event_Implementation(FName EName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void EventWithData(FName EName, UObject* Data);
	void EventWithData_Internal(FName EName, UObject* Data);
	virtual void EventWithData_Implementation(FName EName, UObject* Data);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ProcStateMachine")
	AActor* GetOwner();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ProcStateMachine")
	UProcStateMachine* GetMachine();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void Enter();
	void Enter_Internal();
	virtual void Enter_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void EnterWithData(UObject* Data);
	void EnterWithData_Internal(UObject* Data);
	virtual void EnterWithData_Implementation(UObject* Data);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void Tick(float DeltaTime);
	void Tick_Internal(float DeltaTime);
	virtual void Tick_Implementation(float DeltaTime) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void Exit();
	void Exit_Internal();
	virtual void Exit_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void ExitWithData(UObject* Data);
	void ExitWithData_Internal(UObject* Data);
	virtual void ExitWithData_Implementation(UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine", meta = (HideSelfPin, DefaultToSelf))
	void GoTo(FName State);

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	virtual void SetOwner(UProcStateMachine* Parent);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ProcStateMachine")
	FName GetStateName();

	/* 
	 * Search for a node path in the machine hierarchy. Note this is a task that uses many arrays, and
	 * can be combersome for long paths, so avoid using frequently.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine", meta = (ExpandEnumAsExecs = "Branches"))
	UStateNode* FindNodeByPath(const FString& Path, ESearchResult& Branches);
	virtual UStateNode* FindNodeByPath_Implementation(const FString& Path, ESearchResult& Branches);

	/*
	 * Search for a node path in the machine hierarchy. Note this is a task that uses many arrays, and
	 * can be combersome for long paths, so avoid using frequently.
	 * 
	 * The Path is also in reverse order to speed things up a bit. i.e. if your top level node is A with child C, to Get C, 
	 * the array ['C', 'A'] should be passed.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine", meta = (ExpandEnumAsExecs = "Branches"))
	UStateNode* FindNodeByArray(const TArray<FString>& Path, ESearchResult& Branches);
	virtual UStateNode* FindNodeByArray_Implementation(const TArray<FString>& Path, ESearchResult& Branches);

	FORCEINLINE bool Active() { return this->bActive; }
};


/**
 *
 */
UCLASS(Blueprintable, EditInlineNew)
class CRABTOOLSUE5_API UProcStateMachine : public UObject
{
	GENERATED_BODY()

	// Internal Structure used for keeping track of state transitions and maintain State ID.
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

		int CurrentID() {
			return this->ID;
		}
	} TRANSITION;

	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (AllowPrivateAccess = "true", GetOptions = "StateOptions"))
	FName StartState;

	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (AllowPrivateAccess = "true"))
	TMap<FName, FStateData> Graph;

	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (AllowPrivateAccess = "true"))
	TMap<FName, FAliasData> Aliases;

	UPROPERTY(VisibleAnywhere, Category = "ProcStateMachine", meta = (AllowPrivateAccess = "true"))
	TArray<FName> StateList;

	UPROPERTY(VisibleAnywhere, Category = "ProcStateMachine", meta = (AllowPrivateAccess = "true"))
	FName CurrentStateName;
	TObjectPtr<AActor> Owner;
	TArray<FStateChangeDispatcher> StateChangeEvents;

	void RebindConditions();	

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine")
	void Initialize(AActor* POwner);

	virtual void Initialize_Implementation(AActor* POwner);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ProcStateMachine")
	AActor* GetOwner();

	/* 
	* Resets the state machine to its start state. Does not trigger StateChanged Events.
	*/
	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	void Reset();

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	void Event(FName EName);

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	void EventWithData(FName EName, UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine", meta = (ExpandEnumAsExecs = "Branches"))
	UStateNode* FindNode(FName NodeName, ESearchResult& Branches);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine", meta = (ExpandEnumAsExecs = "Branches"))	
	UStateNode* FindNodeByPath(const FString& Path, ESearchResult& Branches);
	virtual UStateNode* FindNodeByPath_Implementation(const FString& Path, ESearchResult& Branches);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProcStateMachine", meta = (ExpandEnumAsExecs = "Branches"))	
	UStateNode* FindNodeByArray(const TArray<FString>& Path, ESearchResult& Branches);
	virtual UStateNode* FindNodeByArray_Implementation(const TArray<FString>& Path, ESearchResult& Branches);

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	void StateChangeListen(const FStateChangeDispatcher& Callback);

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	void StateChangeObject(UObject* Obj);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ProcStateMachine")
	FName GetStateName(UStateNode* Node);


	/* 
	* Tick function to be called regularly. This is managed by the owner object.
	*/
	void Tick(float DeltaTime);
	/**
	 * This function should not be called except by UStateNode Implementations or unless you
	 * specifically require a state to be forced regardless of the context.
	 */
	void UpdateState(FName Name);
	void UpdateStateWithData(FName Name, UObject* Data);
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;

	FORCEINLINE FStateData* GetCurrentState() { return this->Graph.Find(this->CurrentStateName); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ProcStateMachine")
	FName GetCurrentStateName();

	UFUNCTION()
	TArray<FString> StateOptions();

	UFUNCTION()
	TArray<FString> ConditionOptions();

	UFUNCTION()
	TArray<FString> ConditionDataOptions();

	/**
	 * Function used to ensure proper state setup happens. Only call this if you need to manually initialize a 	 
	 * state machine.
	 */
	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	void Initialize_Internal(AActor* POwner);

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	int GetStateID() { return this->TRANSITION.CurrentID(); }

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	bool IsInState(int ID) { return this->TRANSITION.Valid(ID); }

	/* Condition function that always returns true. */
	UFUNCTION()
	bool TrueCondition();

	UFUNCTION()
	bool FalseCondition();

	/* Condition function that always returns true. */
	UFUNCTION()
	bool TrueDataCondition(UObject* Data);

	UFUNCTION()
	bool FalseDataCondition(UObject* Data);

	/* Condition function that returns true if Data is Valid. */
	UFUNCTION()
	bool ValidDataCondition(UObject* Data);
};
