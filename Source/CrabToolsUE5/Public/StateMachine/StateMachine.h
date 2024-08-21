#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Delegates/Delegate.h"
#include "Templates/UniquePtr.h"
#include "StateChangeListener.h"
#include "Utils/Enums.h"
#include "UObject/ObjectPtr.h"
#include "StateMachine/EventListener.h"
#include "Containers/List.h"
#include "StateMachine.generated.h"

class UStateNode;
class UStateMachine;
class UNodeTransition;
class UStateMachineBlueprintGeneratedClass;


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
struct FStateData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(VisibleAnywhere, Category = "StateMachine")
	TObjectPtr<UStateNode> Node;
	// Map from Event Name to StateName
	UPROPERTY(VisibleAnywhere, Category = "StateMachine")
	TMap<FName, FTransitionData> Transitions;

	UPROPERTY(VisibleAnywhere, Category = "StateMachine")
	TSet<FName> StateClasses;
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

USTRUCT(BlueprintType)
struct FEmittedEvent
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "StateMachine|Events")
	FName EventName;
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
UCLASS(Abstract, Blueprintable, EditInlineNew, CollapseCategories, Category = "StateMachine")
class CRABTOOLSUE5_API UStateNode : public UObject
{
	GENERATED_BODY()

	friend class UStateMachine;

	UPROPERTY()
	TObjectPtr<UStateMachine> Owner;
	bool bActive = false;

	#if WITH_EDITORONLY_DATA
		UPROPERTY(BlueprintReadOnly, Category="StateMachine|Events",
			meta=(AllowPrivateAccess=true))
		TSet<FName> EmittedEvents;
		TSet<FName> PreEditEmittedEvents;
	#endif

public:
	UStateNode();

	/* Function called by Initialize_Internal. Override this to setup your init code. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void Initialize();
	virtual void Initialize_Implementation();

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

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	virtual void SetOwner(UStateMachine* Parent);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	FName GetStateName();

	FORCEINLINE bool Active() { return this->bActive; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void RenameEvent(FName Old, FName New);
	virtual void RenameEvent_Implementation(FName Old, FName New);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void DeleteEvent(FName Event);
	virtual void DeleteEvent_Implementation(FName Event);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void EmitEvent(FName EName);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void EmitEventWithData(FName EName, UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta=(DisplayName="EmitEvent"))
	void EmitEventSlot(const FEventSlot& ESlot);

	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta = (DisplayName = "EmitEventWithData"))
	void EmitEventSlotWithData(const FEventSlot& ESlot, UObject* Data);

	#if WITH_EDITOR
		UFUNCTION()
		TArray<FString> GetEventOptions() const;
		virtual void GetEmittedEvents(TSet<FName>& Events) const;
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
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
 *
 */
UCLASS(Blueprintable, EditInlineNew, Category = "StateMachine")
class CRABTOOLSUE5_API UStateMachine : public UObject, public IEventListenerInterface
{
	GENERATED_BODY()

private:
	// Internal Structure used for keeping track of state transitions and maintain State ID.
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
	} TRANSITION;

	/* The Graph of the state machine. */
	UPROPERTY()
	TMap<FName, FStateData> Graph;

	/* Nodes to be substituted into the graph later. */
	UPROPERTY(EditAnywhere, Instanced, Category = "StateMachine",
		meta = (AllowPrivateAccess = "true"))
	TMap<FName, TObjectPtr<UStateNode>> SharedNodes;

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
	TArray<FStateChangeDispatcher> StateChangeEvents;

	UPROPERTY(EditAnywhere, Instanced, Category="StateMachine",
		meta=(ShowInnerProperties, ShowOnlyInnerProperties, ReadOnlyKeys))
	TMap<FName, TObjectPtr<UStateMachine>> SubMachines;

	/* Reference to the archetype for this SM from a Generated Class. */
	//TObjectPtr<UStateMachine> MachineArchetype;
	/* Reference to a parent which uses this state machine as a sub machine. */
	TObjectPtr<UStateMachine> ParentMachine;
	/* The key/name of this submachine in the parent. */
	FName ParentKey;
	/* Reference to the generated class for this machine. */
	//TSubclassOf<UStateMachine> GeneratedClass;
	/* Sequence of States that this machine has passed through*/
	TDoubleLinkedList<FName> StateStack;

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StateMachine",
		meta = (GetOptions = "StateOptions"))
	FName StartState;

public:

	UStateMachine(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateMachine")
	void Initialize();
	virtual void Initialize_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	AActor* GetOwner();

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void Reset();

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void SendEvent(FName EName) { this->Event_Direct(EName); }
	virtual void Event_Implementation(FName EName) override final { this->Event_Direct(EName); }
	void Event_Direct(FName);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void SendEventWithData(FName EName, UObject* Data) { this->EventWithData_Direct(EName, Data); }
	void EventWithData_Implementation(FName EName, UObject* Data) override final { this->EventWithData_Direct(EName, Data); }
	void EventWithData_Direct(FName EName, UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void StateChangeListen(const FStateChangeDispatcher& Callback);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void StateChangeObject(UObject* Obj);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	FName GetStateName(UStateNode* Node);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine",
		meta = (ExpandEnumAsExecs = "Branches"))
	UStateNode* GetCurrentStateAs(TSubclassOf<UStateNode> Class, ESearchResult& Branches);

	UStateMachine* GetSubMachine(FName MachineKey) { return this->SubMachines.Find(MachineKey)->Get(); }

	/*
	* Tick function to be called regularly. This is managed by the owner object.
	*/
	void Tick(float DeltaTime);

	FStateData* GetCurrentState();
	FStateData* GetStateData(FName Name);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	FName GetCurrentStateName();

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
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

	TSet<FName> GetEvents() const;
	TMap<FName, TObjectPtr<UStateNode>> GetSharedNodes() { return this->SharedNodes; }

	// Procedural constructions functions.
	void AddState(FName StateName);
	void AddTransition(FName State, FName Event, FName Destination, FName Condition, FName DataCondition);
	void AddTransition(FName State, FName Event, FTransitionData Data);
	void ClearStates() { this->Graph.Empty(); }

	void AddStateWithNode(FName StateName, UStateNode* Node);
	void AddStateClass(FName StateName, FName StateClass);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
#endif

protected:

	void CreateDefaultSubMachine(FName Key, TSubclassOf<UStateMachine> Class);

private:

	bool HasEventVariable(FName VName);
	FName GetEventVarName(FName EName);
	void InitFromArchetype();
	void PushStateToStack(FName EName);


	void UpdateState(FName Name);
	void UpdateStateWithData(FName Name, UObject* Data);
	void BindCondition(FTransitionData& Data);

};
