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
#include "StateMachine/Emitters/EventEmitter.h"
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

	UPROPERTY(EditAnywhere, Category = "StateMachine")
	TObjectPtr<UTransitionCondition> Condition;

	UPROPERTY(EditAnywhere, Category = "StateMachine")
	TObjectPtr<UTransitionDataCondition> DataCondition;
};

/*
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
*/

UCLASS(Blueprintable, CollapseCategories, Category = "StateMachine")
class CRABTOOLSUE5_API UState : public UObject
{
	GENERATED_BODY()

	friend class UStateMachine;

	UPROPERTY(DuplicateTransient)
	TObjectPtr<UStateNode> Node;

	// Map from Event Name to StateName
	UPROPERTY(DuplicateTransient)
	TMap<FName, FTransitionData> Transitions;

public:

	/* Only use to build states. Do not use for currently in use States/State Machines */
	void Append(UState* Data);
	/* Only use to build states. Do not use for currently in use States/State Machines */
	void AppendCopy(UState* Data);
	/* Only use to build states. Do not use for currently in use States/State Machines */
	void AppendNode(UStateNode* Node);
	/* Only use to build states. Do not use for currently in use States/State Machines */
	void AppendNodeCopy(UStateNode* Node);

	UFUNCTION(BlueprintCallable, Category="StateMachine")
	FORCEINLINE UStateNode* GetNode() const { return this->Node; }
	FORCEINLINE const TMap<FName, FTransitionData>& GetTransitions() const { return Transitions; }
};

UCLASS(BlueprintType, Abstract, Category = "StateMachine")
class CRABTOOLSUE5_API UTransitionCondition : public UObject
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TObjectPtr<UStateMachine> Owner;

public:

	void Initialize(UStateMachine* NewOwner);
	virtual bool Check() const { return false; }

	UFUNCTION(BlueprintCallable, Category="StateMachine|Transition")
	FORCEINLINE UStateMachine* GetOwner() const { return this->Owner; }

protected:

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine|Transition")
	void Initialize_Inner();
	virtual void Initialize_Inner_Implementation() {}
};

UCLASS(BlueprintType, Abstract, Category = "StateMachine")
class CRABTOOLSUE5_API UTransitionDataCondition : public UObject
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	TObjectPtr<UStateMachine> Owner;

public:

	void Initialize(UStateMachine* Owner);
	virtual bool Check(UObject* Data) const { return false; }

	UFUNCTION(BlueprintCallable, Category = "StateMachine|Transition")
	FORCEINLINE UStateMachine* GetOwner() const { return this->Owner; }

protected:

	UFUNCTION(BlueprintNativeEvent, Category="StateMachine|Transition")
	void Initialize_Inner();
	virtual void Initialize_Inner_Implementation() {}
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

	UPROPERTY(Transient)
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
	void Initialize(UStateMachine* POwner);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	virtual void SetOwner(UStateMachine* Parent);

	FORCEINLINE bool Active() const { return this->bActive; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	AActor* GetOwner();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	UStateMachine* GetMachine() const;

	UFUNCTION(BlueprintCallable, Category = "RPG", meta = (ExpandEnumAsExecs = "Result", DeterminesOutputType = "SClass"))
	UStateMachine* GetMachineAs(TSubclassOf<UStateMachine> SClass, ESearchResult& Result) const;

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

	void Event(FName EName);
	void EventWithData(FName EName, UObject* Data);
	void Enter();
	void EnterWithData(UObject* Data);
	void Tick(float DeltaTime);
	void Exit();
	void ExitWithData(UObject* Data);
	void PostTransition();
	void SetActive(bool bNewActive) { this->bActive = bNewActive; }

	/* Runs a verification check on the node. Returns true if no error, false if an error happened. */
	bool Verify(FNodeVerificationContext& Context) const;

protected:

	/* Override this with your verification code. */
	virtual bool Verify_Inner(FNodeVerificationContext& Context) const { return true; }

	/* Function called by Initialize_Internal. Override this to setup your init code. */
	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void Initialize_Inner();
	virtual void Initialize_Inner_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void Event_Inner(FName EName);	
	virtual void Event_Inner_Implementation(FName EName);

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void EventWithData_Inner(FName EName, UObject* Data);
	virtual void EventWithData_Inner_Implementation(FName EName, UObject* Data);

	/* Call the _Internal Version. */
	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void Enter_Inner();	
	virtual void Enter_Inner_Implementation() {}

	/* Call the _Internal Version. */
	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void EnterWithData_Inner(UObject* Data);	
	virtual void EnterWithData_Inner_Implementation(UObject* Data);

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void Tick_Inner(float DeltaTime);	
	virtual void Tick_Inner_Implementation(float DeltaTime) {}

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void Exit_Inner();
	virtual void Exit_Inner_Implementation() {}

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void PostTransition_Inner();
	virtual void PostTransition_Inner_Implementation() {}

	/* Call the _Internal Version. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void ExitWithData_Inner(UObject* Data);	
	virtual void ExitWithData_Inner_Implementation(UObject* Data);

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

		UFUNCTION()
		TArray<FString> GetStateOptions() const;

		UFUNCTION()
		TArray<FString> GetIncomingStateOptions() const;

		UFUNCTION()
		TArray<FString> GetOutgoingStateOptions() const;
	#endif

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
	UPROPERTY(meta=(IgnorePropertySearch))
	TMap<FName, TObjectPtr<UState>> Graph;

	/* Nodes to be substituted into the graph later. */
	UPROPERTY(EditAnywhere, Instanced, Category = "StateMachine",
		meta = (AllowPrivateAccess, IgnorePropertySearch))
	TMap<FName, TObjectPtr<UStateNode>> SharedNodes;

	/* State Machines to be substituted into the graph later. */
	UPROPERTY(EditAnywhere, Instanced, Category = "StateMachine",
		meta = (AllowPrivateAccess, IgnorePropertySearch))
	TMap<FName, TObjectPtr<UStateNode>> SharedMachines;

	UPROPERTY(BlueprintReadOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess, IgnorePropertySearch))
	FName CurrentStateName;

	UPROPERTY(EditDefaultsOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess, IgnorePropertySearch))
	TArray<UEventEmitter*> EventEmitters;

	/* How many previous states to remember. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StateMachine",
		meta = (AllowPrivateAccess, IgnorePropertySearch,
			ClampMin = "2", ClampMax = "1000", UIMin = "2", UIMax = "1000"))
	int MaxStackSize = 5;

	UPROPERTY(meta=(IgnorePropertySearch))
	TObjectPtr<AActor> Owner;

	/* Map of SubMachines. Do not change the root value of the SM, only its subproperties. */
	UPROPERTY(meta=(IgnorePropertySearch))
	TMap<FName, TObjectPtr<UStateMachine>> SubMachines;

	/* Reference to a parent which uses this state machine as a sub machine. */
	TObjectPtr<UStateMachine> ParentMachine;
	/* The key/name of this submachine in the parent. */
	FName ParentKey;
	/* Sequence of States that this machine has passed through*/
	TDoubleLinkedList<FName> StateStack;

public:

	UPROPERTY(meta=(IgnorePropertySearch))
	FName StartState;

	UPROPERTY(BlueprintAssignable, Category="StateMachine", meta = (IgnorePropertySearch))
	FStateChangedEvent OnStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "StateMachine", meta = (IgnorePropertySearch))
	FTransitionFinishedEvent OnTransitionFinished;

public:

	UStateMachine(const FObjectInitializer& ObjectInitializer);

	const TArray<UEventEmitter*>& GetEmitters() const { return this->EventEmitters; }

	/**
	 * Function used to ensure proper state setup happens. Only call this if you need to manually initialize a
	 * state machine.
	 */
	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void Initialize(AActor* POwner);

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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	UState* GetCurrentStateData() const;

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

	// Procedural construction functions.
	void AddState(FName StateName);
	void AddTransition(FName State, FName Event, FName Destination, UTransitionCondition* Condition, UTransitionDataCondition* DataCondition);
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
		void CollectExtendibleStates(TSet<FString>& StateNames) const;
	#endif

	// IStateMachineLike interface
	virtual TArray<FString> GetStateOptions(const UObject* Asker) const override;
	virtual TArray<FString> GetPropertiesOptions(FSMPropertySearch& SearchParam) const override;
	virtual FSMPropertyReference GetStateMachineProperty(FString& Address) const override;
	virtual IStateMachineLike* GetSubMachine(FString& Address) const override;

	UFUNCTION(BlueprintCallable, Category="StateMachine")
	bool IsTransitioning() const { return this->bIsTransitioning; }

	/* Used to bind transition delegates. */
	void BindConditionAt(FString& Address, FTransitionDelegate& Condition);
	/* Used to bind transition delegates. */
	void BindDataConditionAt(FString& Address, FTransitionDataDelegate& Condition);

	UState* DuplicateStateObject(FName StateName, UObject* NewOuter) const;

	/* Returns the names of states that can be extended, but not overwritten. */
	TArray<FString> GetExtendibleStates() const;

	/* Returns the names of states that can be extended, but not overwritten. */
	TArray<FString> GetOverrideableStates() const;

protected:

	UFUNCTION(BlueprintNativeEvent, Category = "StateMachine")
	void Initialize_Inner();
	virtual void Initialize_Inner_Implementation();

private:

	void InitFromArchetype();
	void PushStateToStack(FName EName);
	void UpdateState(FName Name);
	void UpdateStateWithData(FName Name, UObject* Data);	
	void InitSubMachines();
};
