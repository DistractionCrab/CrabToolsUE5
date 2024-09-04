#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Delegates/Delegate.h"
#include "Templates/UniquePtr.h"
#include "StateChangeListener.h"
#include "Utils/Enums.h"
#include "UObject/ObjectPtr.h"
#include "StateMachine/EventListener.h"
#include "StateMachine/IStateMachineLike.h"
#include "StateMachine/StateMachineEnum.h"
#include "Containers/List.h"
#include "StateMachine.generated.h"

class UStateNode;
class UStateMachine;
class UNodeTransition;
class UStateMachineBlueprintGeneratedClass;

/* Structure used to pass to listeners for when states change.*/
USTRUCT(BlueprintType)
struct FStateChangedEventData
{
	GENERATED_USTRUCT_BODY()

	/* Which state we transitioned from. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="StateMachine")
	FName From;

	/* Which state we have transitioned into. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateMachine")
	FName To;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateMachine")
	TObjectPtr<UState> FromState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateMachine")
	TObjectPtr<UState> ToState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateMachine")
	TObjectPtr<UStateMachine> StateMachine;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStateChangedEvent, const FStateChangedEventData&, Data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTransitionFinishedEvent, UStateMachine*, Machine);
DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FTransitionDelegate);
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FTransitionDataDelegate, UObject*, Data);


/* 
 * Simple interface to use for external modules to relay messages when verifying Node integrity. 
 * This is meant to mimic a KismetCompilerContext somewhat, but exists specifically to avoid the issue
 * of needing an editor only interface in the runtime.
 */
class FNodeVerificationContext
{
private:

	TObjectPtr<UClass> TargetClass;

public:

	FNodeVerificationContext(UClass* Class) : TargetClass(Class) {}

	virtual void Error(FString& Msg, const UObject* Obj) = 0;
	virtual void Warning(FString& Msg, const UObject* Obj) = 0;
	virtual void Note(FString& Msg, const UObject* Obj) = 0;

	UObject* GetOuter() const { return this->TargetClass; }
};

USTRUCT(BlueprintType)
struct FTransitionData
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
struct FStateMachineEventRef
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

/* Structure used to store an event to be used by a node. */
USTRUCT(BlueprintType)
struct FEventSlot
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category="StateMachine|Events",
		meta=(GetOptions="GetEventOptions"))
	FName EventName;
};

/* Structure used to store a reference to a submachine. */
USTRUCT(BlueprintType)
struct FSubMachineSlot
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "StateMachine",
		meta = (GetOptions = "GetMachineOptions"))
	FName MachineName;
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

UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine")
class CRABTOOLSUE5_API UState : public UObject
{
	GENERATED_BODY()

	friend class UStateMachine;

	UPROPERTY()
	TObjectPtr<UStateNode> Node;

	// Map from Event Name to StateName
	UPROPERTY()
	TMap<FName, FTransitionData> Transitions;

	UPROPERTY(EditDefaultsOnly, Category="StateMachine")
	EStateMachineAccessibility Access = EStateMachineAccessibility::PRIVATE;

public:

	/* Only use to build states. Do not use for currently in use States/State Machines */
	void Append(UState* Data);
	/* Only use to build states. Do not use for currently in use States/State Machines */
	void AppendCopy(UState* Data);
	/* Only use to build states. Do not use for currently in use States/State Machines */
	void AppendNode(UStateNode* Node);
	/* Only use to build states. Do not use for currently in use States/State Machines */
	void AppendNodeCopy(UStateNode* Node);

	FORCEINLINE EStateMachineAccessibility GetAccess() const { return this->Access; }

	UFUNCTION(BlueprintCallable, Category="StateMachine")
	FORCEINLINE UStateNode* GetNode() const { return this->Node; }
	const TMap<FName, FTransitionData>& GetTransitions() const { return Transitions; }
};

/**
 * Base Node class for an individual node in a statemachine. Defines the general behaviour and interface
 * for what nodes can do.
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, CollapseCategories, Category = "StateMachine")
class CRABTOOLSUE5_API UStateNode : public UObject
{
	GENERATED_BODY()

	friend class UStateMachine;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="StateMachine",
		meta=(AllowPrivateAccess=true))
	bool bRequiresTick = false;

	UPROPERTY()
	TObjectPtr<UStateMachine> Owner;
	bool bActive = false;

	#if WITH_EDITORONLY_DATA
		UPROPERTY(EditDefaultsOnly, Category="StateMachine|Events",
			meta=(AllowPrivateAccess=true, HideInDetailPanel))
		TSet<FName> EmittedEvents;
		TSet<FName> PreEditEmittedEvents;
	#endif

public:
	UStateNode();

	/**
	 * Function used to ensure proper state setup happens. Only call this if you need to
	 * manually initialize a state machine.
	 */
	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void Initialize_Internal(UStateMachine* POwner);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	virtual void SetOwner(UStateMachine* Parent);

	FORCEINLINE bool Active() { return this->bActive; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	AActor* GetOwner();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	UStateMachine* GetMachine();

	UFUNCTION(BlueprintCallable, Category = "RPG", meta = (ExpandEnumAsExecs = "Result", DeterminesOutputType = "SClass"))
	UStateMachine* GetMachineAs(TSubclassOf<UStateMachine> SClass, ESearchResult& Result);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void EmitEvent(FName EName);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void EmitEventWithData(FName EName, UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta=(DisplayName="EmitEvent"))
	void EmitEventSlot(const FEventSlot& ESlot);

	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta = (DisplayName = "EmitEventWithData"))
	void EmitEventSlotWithData(const FEventSlot& ESlot, UObject* Data);

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	bool RequiresTick() const;
	virtual bool RequiresTick_Implementation() const { return this->bRequiresTick; }

	#if WITH_EDITOR
		virtual void GetEmittedEvents(TSet<FName>& Events) const;
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	#endif

	void Event_Internal(FName EName);
	void EventWithData_Internal(FName EName, UObject* Data);
	void Enter_Internal();
	void EnterWithData_Internal(UObject* Data);
	void Tick_Internal(float DeltaTime);
	void Exit_Internal();
	void ExitWithData_Internal(UObject* Data);
	void PostTransition_Internal();
	void SetActive(bool bNewActive) { this->bActive = bNewActive; }

	/* Runs a verification check on the node. Returns true if no error, false if an error happened. */
	virtual bool Verify(FNodeVerificationContext& Context) const;

protected:

	/* Function called by Initialize_Internal. Override this to setup your init code. */
	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void Initialize();
	virtual void Initialize_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void Event(FName EName);	
	virtual void Event_Implementation(FName EName);

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void EventWithData(FName EName, UObject* Data);
	
	virtual void EventWithData_Implementation(FName EName, UObject* Data);

	/* Call the _Internal Version. */
	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void Enter();	
	virtual void Enter_Implementation() {}

	/* Call the _Internal Version. */
	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void EnterWithData(UObject* Data);	
	virtual void EnterWithData_Implementation(UObject* Data);

	/* Call the _Internal Version. */
	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void Tick(float DeltaTime);	
	virtual void Tick_Implementation(float DeltaTime) {}

	/* Call the _Internal Version. */
	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void Exit();
	/* Call the _Internal Version. */
	virtual void Exit_Implementation() {}

	/* Call the _Internal Version. */
	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void PostTransition();
	virtual void PostTransition_Implementation() {}

	/* Call the _Internal Version. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void ExitWithData(UObject* Data);	
	virtual void ExitWithData_Implementation(UObject* Data);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void RenameEvent(FName Old, FName New);
	virtual void RenameEvent_Implementation(FName Old, FName New);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void DeleteEvent(FName Event);
	virtual void DeleteEvent_Implementation(FName Event);

	#if WITH_EDITOR
		UFUNCTION()
		TArray<FString> GetEventOptions() const;

		UFUNCTION()
		TArray<FString> GetMachineOptions() const;
	#endif

protected:

	void AddEmittedEvent(FName Event)
	{
		#if WITH_EDITORONLY_DATA
			this->EmittedEvents.Add(Event);
		#endif
	}
};


/**
 * State Machine class. This is the base class for any State Machine, and manages all
 * appropriate state machine behaviour.
 */
UCLASS(Blueprintable, EditInlineNew, Category = "StateMachine")
class CRABTOOLSUE5_API UStateMachine : public UObject, public IEventListenerInterface, public IStateMachineLike
{
	GENERATED_BODY()

private:
	/* Struct used for identifying unique states. */
	struct Transition
	{
	private:
		int ID = 0;

	public:
		int EnterTransition()
		{
			this->ID += 1;
			return ID;
		}

		bool Valid(int OID)
		{
			return OID == this->ID;
		}

		int CurrentID()
		{
			return this->ID;
		}
	} TransitionIdentifier;

	/* Whether or not a transition is happening. */
	bool bIsTransitioning = false;

	/* The Graph of the state machine. */
	UPROPERTY()
	TMap<FName, TObjectPtr<UState>> Graph;

	/* Nodes to be substituted into the graph later. */
	UPROPERTY(EditAnywhere, Instanced, Category = "StateMachine",
		meta = (AllowPrivateAccess = "true"))
	TMap<FName, TObjectPtr<UStateNode>> SharedNodes;

	/* State Machines to be substituted into the graph later. */
	UPROPERTY(EditAnywhere, Instanced, Category = "StateMachine",
		meta = (AllowPrivateAccess = "true"))
	TMap<FName, TObjectPtr<UStateNode>> SharedMachines;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess = "true"))
	FName CurrentStateName;

	/* How many previous states to remember. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess = "true",
			ClampMin = "2", ClampMax = "1000", UIMin = "2", UIMax = "1000"))
	int MaxStackSize = 5;

	UPROPERTY()
	TObjectPtr<AActor> Owner;

	/* Map of SubMachines. Do not change the root value of the SM, only its subproperties. */
	UPROPERTY()
	TMap<FName, TObjectPtr<UStateMachine>> SubMachines;

	/* Reference to a parent which uses this state machine as a sub machine. */
	TObjectPtr<UStateMachine> ParentMachine;
	/* The key/name of this submachine in the parent. */
	FName ParentKey;
	/* Sequence of States that this machine has passed through*/
	TDoubleLinkedList<FName> StateStack;

public:

	UPROPERTY()
	FName StartState;

	UPROPERTY(BlueprintAssignable, Category="StateMachine")
	FStateChangedEvent OnStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "StateMachine")
	FTransitionFinishedEvent OnTransitionFinished;

public:

	UStateMachine(const FObjectInitializer& ObjectInitializer);

	/**
	 * Function used to ensure proper state setup happens. Only call this if you need to manually initialize a
	 * state machine.
	 */
	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void Initialize_Internal(AActor* POwner);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void SetActive(bool bNewActive);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	AActor* GetOwner();

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void Reset();

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void SendEvent(FName EName);
	virtual void Event_Implementation(FName EName) override final { this->SendEvent(EName); }

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void SendEventWithData(FName EName, UObject* Data);
	void EventWithData_Implementation(FName EName, UObject* Data) override final { this->SendEventWithData(EName, Data); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine",
		meta = (ExpandEnumAsExecs = "Branches"))
	UStateNode* GetCurrentStateAs(TSubclassOf<UStateNode> Class, ESearchResult& Branches);

	/*
	* Tick function to be called regularly. This is managed by the owner object.
	*/
	void Tick(float DeltaTime);

	UState* GetCurrentState();
	UState* GetStateData(FName Name);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	FName GetCurrentStateName();

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	TArray<FString> StateOptions();

	UFUNCTION()
	TArray<FString> ConditionOptions();

	UFUNCTION()
	TArray<FString> ConditionDataOptions();

	

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	FName GetPreviousState() const;


	/* Condition function that always returns true. */
	UFUNCTION(meta = (IsDefaultCondition))
	bool TrueCondition();

	UFUNCTION(meta = (IsDefaultCondition))
	bool FalseCondition();

	/* Condition function that always returns true. */
	UFUNCTION(meta = (IsDefaultCondition))
	bool TrueDataCondition(UObject* Data);

	UFUNCTION(meta = (IsDefaultCondition))
	bool FalseDataCondition(UObject* Data);

	/* Condition function that returns true if Data is Valid. */
	UFUNCTION(meta = (IsDefaultCondition))
	bool ValidDataCondition(UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	int GetStateID() { return this->TransitionIdentifier.CurrentID(); }

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	bool IsInState(int ID) { return this->TransitionIdentifier.Valid(ID); }

	TSet<FName> GetEvents() const;
	TMap<FName, TObjectPtr<UStateNode>> GetSharedNodes() { return this->SharedNodes; }

	// Procedural constructions functions.
	void AddState(FName StateName);
	void AddTransition(FName State, FName Event, FName Destination, FName Condition, FName DataCondition);
	void AddTransition(FName State, FName Event, FTransitionData Data);
	void ClearStates() { this->Graph.Empty(); }

	void AddStateData(FName StateName, UState* Data);
	void AddStateWithNode(FName StateName, UStateNode* Node);
	void SetParentData(UStateMachine* Parent, FName NewParentKey);
	UStateMachineBlueprintGeneratedClass* GetGeneratedClass() const;
	TArray<FString> GetStatesWithAccessibility(EStateMachineAccessibility Access) const;


	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
		virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
		virtual void PostLinkerChange() override;
	#endif

	// IStateMachineLike interface
	virtual TArray<FString> GetStateOptions(const UObject* Asker) const override;
	virtual TArray<FString> GetPropertiesOptions(FSMPropertySearch& SearchParam) const override;
	virtual FProperty* GetStateMachineProperty(FString& Address) const override;
	virtual IStateMachineLike* GetSubMachine(FString& Address) const override;

	UFUNCTION(BlueprintCallable, Category="StateMachine")
	bool IsTransitioning() const { return this->bIsTransitioning; }

protected:

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void Initialize();
	virtual void Initialize_Implementation();

private:

	bool HasEventVariable(FName VName) const;
	FName GetEventVarName(FName EName);
	void InitFromArchetype();
	void PushStateToStack(FName EName);
	void UpdateState(FName Name);
	void UpdateStateWithData(FName Name, UObject* Data);	
	void InitSubMachines();
	void BindCondition(FTransitionData& Data);
	void BindConditionAt(FString& Address, FTransitionData& Data);
	void BindDataConditionAt(FString& Address, FTransitionData& Data);
};
