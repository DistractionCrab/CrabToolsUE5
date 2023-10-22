// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Delegates/Delegate.h"
#include "Templates/UniquePtr.h"
#include "ProcStateMachine.generated.h"

class UStateNode;
class UProcStateMachine;
class UNodeTransition;

UENUM(BlueprintType)
enum class ENodeSearchResult : uint8 {
	FOUND          UMETA(DisplayName = "Found"),
	NOTFOUND       UMETA(DisplayName = "Missing"),
};

USTRUCT(BlueprintType)
struct  FTransitionData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, Instanced, Category = "ProcStateMachine")
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
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (GetValueOptions = "StateOptions"))
	TMap<FName, FName> EventTransitions;
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine")
	TArray<FTransitionData> Transitions;
};

/**
 *
 */
UCLASS(Blueprintable, EditInlineNew)
class CRABTOOLSUE5_API UStateNode : public UObject
{
	GENERATED_BODY()

	UProcStateMachine* Owner;

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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ProcStateMachine")
	AActor* GetOwner();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ProcStateMachine")
	UProcStateMachine* GetMachine();
};


USTRUCT(BlueprintType)
struct FAliasData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta=(GetOptions="StateOptions"))
	TSet<FName> States;

	// Mapping of EventName -> StateName.
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (GetValueOptions="StateOptions"))
	TMap<FName, FName> Transitions;
	
};


DECLARE_DYNAMIC_DELEGATE_TwoParams(FStateChangeDispatcher, FName, From, FName, To);
/**
 *
 */
UCLASS(Blueprintable, EditInlineNew)
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
	UPROPERTY(EditAnywhere, Category = "ProcStateMachine", meta = (AllowPrivateAccess = "true"))
	TMap<FName, FAliasData> Aliases;
	UPROPERTY(VisibleAnywhere, Category = "ProcStateMachine", meta = (AllowPrivateAccess = "true"))
	TArray<FName> StateList;

	UPROPERTY(VisibleAnywhere, Category = "ProcStateMachine", meta = (AllowPrivateAccess = "true"))
	FName CurrentStateName;
	AActor* Owner;
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
	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine", meta = (ExpandEnumAsExecs = "Branches"))
	UStateNode* FindNode(FName NodeName, ENodeSearchResult& Branches);
	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	void StateChangeListen(const FStateChangeDispatcher& Callback);
	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
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
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;

	FORCEINLINE FStateData* GetCurrentState() { return this->Graph.Find(this->CurrentStateName); }

	UFUNCTION()
	TArray<FString> StateOptions() {
		UE_LOG(LogTemp, Warning, TEXT("Querying for State Options."));
		TArray<FString> Names;
		for (const auto& Nodes : this->Graph) {
			Names.Add(Nodes.Key.ToString());
		}
		return Names;
	}
};
