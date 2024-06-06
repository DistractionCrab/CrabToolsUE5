// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Delegates/Delegate.h"
#include "Templates/UniquePtr.h"
#include "StateChangeListener.h"
#include "Utils/Enums.h"
#include "UObject/ObjectPtr.h"
#include "StateMachine.generated.h"

class UStateNode;
class UStateMachine;
class UNodeTransition;


DECLARE_DYNAMIC_DELEGATE_TwoParams(FStateChangeDispatcher, FName, From, FName, To);
DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FTransitionDelegate);
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FTransitionDataDelegate, UObject*, Data);

USTRUCT(BlueprintType)
struct  FTransitionData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (GetOptions = "StateOptions"))
	FName Destination;
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (GetOptions = "ConditionOptions"))
	FName Condition = "TrueCondition";
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (GetOptions = "ConditionDataOptions"))
	FName DataCondition = "TrueDataCondition";
	
	FTransitionDelegate ConditionCallback;
	FTransitionDataDelegate DataConditionCallback;
};

USTRUCT(BlueprintType, meta = (DisableSplitPin))
struct  FStateMachineEventRef
{
	GENERATED_USTRUCT_BODY()

	TWeakObjectPtr<UStateMachine> Owner;

	UPROPERTY(VisibleAnywhere, Category = "StateMachine|Events")
	FName EventName;

public:

	FStateMachineEventRef() {}
	FStateMachineEventRef(FName EName) { this->EventName = EName; }

	void Activate();
	void ActivateWithData(UObject* Data);
};



USTRUCT(BlueprintType)
struct  FStateData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Instanced, Category = "StateMachine")
	TObjectPtr<UStateNode> Node;
	// Map from Event Name to StateName
	UPROPERTY(EditAnywhere, Category = "StateMachine")
	TMap<FName, FTransitionData> Transitions;
};

USTRUCT(BlueprintType)
struct FAliasData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (GetOptions = "StateOptions"))
	TSet<FName> States;

	// Mapping of EventName -> TransitionData.
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (GetValueOptions = "StateOptions"))
	TMap<FName, FTransitionData> Transitions;

};

/**
 *
 */
UCLASS(Blueprintable, Abstract, EditInlineNew, CollapseCategories, Category="StateMachine")
class CRABTOOLSUE5_API UStateNode : public UObject
{
	GENERATED_BODY()

	friend class UStateMachine;

	UPROPERTY()
	TObjectPtr<UStateMachine> Owner;
	bool bActive = false;

	

public:

	/* Function called by Initialize_Internal. Override this to setup your init code. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void Initialize(UStateMachine* POwner);
	virtual void Initialize_Implementation(UStateMachine* POwner);

	/**
	 * Function used to ensure proper state setup happens. Only call this if you need to manually initialize a
	 * state machine.
	 */
	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void Initialize_Internal(UStateMachine* POwner);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void Event(FName EName);
	void Event_Internal(FName EName);
	virtual void Event_Implementation(FName EName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void EventWithData(FName EName, UObject* Data);
	void EventWithData_Internal(FName EName, UObject* Data);
	virtual void EventWithData_Implementation(FName EName, UObject* Data);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	AActor* GetOwner();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	UStateMachine* GetMachine();

	UFUNCTION(BlueprintCallable, Category = "RPG", meta = (ExpandEnumAsExecs = "Result", DeterminesOutputType = "SClass"))
	UStateMachine* GetMachineAs(TSubclassOf<UStateMachine> SClass, ESearchResult& Result);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void Enter();
	void Enter_Internal();
	virtual void Enter_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void EnterWithData(UObject* Data);
	void EnterWithData_Internal(UObject* Data);
	virtual void EnterWithData_Implementation(UObject* Data);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void Tick(float DeltaTime);
	void Tick_Internal(float DeltaTime);
	virtual void Tick_Implementation(float DeltaTime) {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void Exit();
	void Exit_Internal();
	virtual void Exit_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void ExitWithData(UObject* Data);
	void ExitWithData_Internal(UObject* Data);
	virtual void ExitWithData_Implementation(UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta = (HideSelfPin, DefaultToSelf))
	void GoTo(FName State);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	virtual void SetOwner(UStateMachine* Parent);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	FName GetStateName();

	/* 
	 * Search for a node path in the machine hierarchy. Note this is a task that uses many arrays, and
	 * can be combersome for long paths, so avoid using frequently.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine", meta = (ExpandEnumAsExecs = "Branches"))
	UStateNode* FindNodeByPath(const FString& Path, ESearchResult& Branches);
	virtual UStateNode* FindNodeByPath_Implementation(const FString& Path, ESearchResult& Branches);

	/*
	 * Search for a node path in the machine hierarchy. Note this is a task that uses many arrays, and
	 * can be combersome for long paths, so avoid using frequently.
	 * 
	 * The Path is also in reverse order to speed things up a bit. i.e. if your top level node is A with child C, to Get C, 
	 * the array ['C', 'A'] should be passed.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine", meta = (ExpandEnumAsExecs = "Branches"))
	UStateNode* FindNodeByArray(const TArray<FString>& Path, ESearchResult& Branches);
	virtual UStateNode* FindNodeByArray_Implementation(const TArray<FString>& Path, ESearchResult& Branches);

	FORCEINLINE bool Active() { return this->bActive; }

	void GetEvents(TSet<FName>& List);
	virtual UStateNode* Substitute(FName SlotName, UStateNode* Node);

	virtual UStateNode* ExtractAs(TSubclassOf<UStateNode> Class);
};


/**
 *
 */
UCLASS(Blueprintable, EditInlineNew, Category = "StateMachine")
class CRABTOOLSUE5_API UStateMachine : public UObject
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

	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess = "true", GetOptions = "StateOptions"))
	FName StartState;

	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess = "true"))
	TMap<FName, FStateData> Graph;

	UPROPERTY(EditAnywhere, Instanced, Category = "StateMachine", meta = (AllowPrivateAccess = "true"))
	TMap<FName, UStateNode*> SharedNodes;

	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess = "true"))
	TMap<FName, FAliasData> Aliases;

	UPROPERTY(VisibleAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess = "true"))
	TArray<FName> StateList;

	UPROPERTY(VisibleAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess = "true"))
	FName CurrentStateName;
	TObjectPtr<AActor> Owner;
	TArray<FStateChangeDispatcher> StateChangeEvents;

	void RebindConditions();
	void ValidateEventProps();
	void AddEventRefStruct(UBlueprint* BlueprintAsset, FName VName, FName EName);
	bool HasEventVariable(FName VName);
	FName GetEventVarName(FName EName);

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void Initialize(AActor* POwner);

	virtual void Initialize_Implementation(AActor* POwner);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	AActor* GetOwner();

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void Reset();

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void Event(FName EName);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void EventWithData(FName EName, UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta = (ExpandEnumAsExecs = "Branches"))
	UStateNode* FindNode(FName NodeName, ESearchResult& Branches);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine", 
		meta = (ExpandEnumAsExecs = "Branches"))	
	UStateNode* FindNodeByPath(const FString& Path, ESearchResult& Branches);
	virtual UStateNode* FindNodeByPath_Implementation(const FString& Path, ESearchResult& Branches);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine", 
		meta=(ExpandEnumAsExecs = "Branches"))
	UStateNode* FindNodeByArray(const TArray<FString>& Path, ESearchResult& Branches);
	virtual UStateNode* FindNodeByArray_Implementation(const TArray<FString>& Path, ESearchResult& Branches);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void StateChangeListen(const FStateChangeDispatcher& Callback);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void StateChangeObject(UObject* Obj);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	FName GetStateName(UStateNode* Node);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine",
		meta = (ExpandEnumAsExecs = "Branches"))
	UStateNode* GetCurrentStateAs(TSubclassOf<UStateNode> Class, ESearchResult& Branches);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine",
		meta = (ExpandEnumAsExecs = "Branches"))
	UStateNode* FindCurrentStateAs(TSubclassOf<UStateNode> Class, ESearchResult& Branches);

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
	virtual void PostCDOCompiled(const FPostCDOCompiledContext& Context) override;
	virtual void PostCDOContruct() override;
	
	FORCEINLINE FStateData* GetCurrentState() { return this->Graph.Find(this->CurrentStateName); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	FName GetCurrentStateName();

	UFUNCTION(BlueprintCallable, Category="StateMachine")
	TArray<FString> StateOptions();

	UFUNCTION()
	TArray<FString> ConditionOptions();

	UFUNCTION()
	TArray<FString> ConditionDataOptions();

	/**
	 * Function used to ensure proper state setup happens. Only call this if you need to manually initialize a 	 
	 * state machine.
	 */
	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void Initialize_Internal(AActor* POwner);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	int GetStateID() { return this->TRANSITION.CurrentID(); }

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
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

	TSet<FName> GetEvents() const;

	void Substitute(FName SlotName, UStateNode* Node);

	// Procedural constructions functions.
	void AddState(FName StateName);

};
