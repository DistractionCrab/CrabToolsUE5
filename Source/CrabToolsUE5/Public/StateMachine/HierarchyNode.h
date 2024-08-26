#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateMachine.h"
#include "HierarchyNode.generated.h"

UENUM(BlueprintType)
enum class EHierarchyInputType : uint8
{
	/* Use an event or state machine that is defined in the blueprint. */
	DEFINED       UMETA(DisplayName = "Defined"),
	/* Use an event or state machine that is inlined in this slot. */
	INLINED       UMETA(DisplayName = "Inlined"),
};

USTRUCT(BlueprintType)
struct FHierarchyEventValue
{
	GENERATED_USTRUCT_BODY()

	/* Use custom inlined event? */
	UPROPERTY(EditDefaultsOnly, Category = "StateMachine")
	EHierarchyInputType EventType = EHierarchyInputType::INLINED;

	UPROPERTY(EditDefaultsOnly, Category="StateMachine|Events",
		meta=(EditCondition="EventType == EHierarchyInputType::INLINED", EditConditionHides))
	FName InlinedEvent;


	UPROPERTY(EditDefaultsOnly, Category = "StateMachine|Events",
		meta = (EditCondition = "EventType == EHierarchyInputType::DEFINED", EditConditionHides,
			GetOptions="GetStateEventOptions"))
	FName DefinedEvent;

	FName GetEvent() const;
};

/**
 * State Machine node used that is controlled by a StateMachine.
 */
UCLASS(Category = "StateMachine")
class CRABTOOLSUE5_API UHierarchyNode : public UStateNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category="StateMachine")
	EHierarchyInputType StateMachineSource = EHierarchyInputType::DEFINED;

	UPROPERTY(EditAnywhere, Instanced, Category = "StateMachine", 
		meta = (AllowPrivateAccess = "true", ShowInnerProperties, ShowOnlyInnerProperties,
			EditCondition = "StateMachineSource == EHierarchyInputType::INLINED", EditConditionHides))
	TObjectPtr<UStateMachine> SubMachine;

	/* Name of the submachine to be placed in this node. */
	UPROPERTY(EditAnywhere, Category="StateMachine",
		meta=(GetOptions="GetMachineOptions", EditCondition= "StateMachineSource == EHierarchyInputType::DEFINED", EditConditionHides))
	FName SlotName;

	/* Map of SubMachine states and events to be emitted to the SM of this node. */
	UPROPERTY(EditAnywhere, Category = "StateMachine", 
		meta = (AllowPrivateAccess = "true", 
			GetKeyOptions="GetSubMachineStateOptions", ForceInlineRow, ShowOnlyInnerProperties))
	TMap<FName, FHierarchyEventValue> ExitStates;

	/* Whether or not the submachine should be reset when this node is entered. */
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess = "true"))
	bool ResetOnEnter = true;

	/*
	 * The event to pass to the submachine when entering. Useful for when ResetOnEnter is false, but
	 * work needs to be continued. Specifically, if ResetOnEnter is false, and nothing is done to transition 
	 * the state, on the next tick or event the exist state will be detected again.
	 */
	UPROPERTY(EditAnywhere, Category = "StateMachine", meta = (AllowPrivateAccess = "true"))
	FName EnterEventName;

public:
	virtual void Initialize_Implementation() override;
	virtual void Event_Implementation(FName EName) override;
	virtual void EventWithData_Implementation(FName EName, UObject* Data) override;
	virtual void Enter_Implementation() override;
	virtual void Tick_Implementation(float DeltaTime) override;
	virtual void Exit_Implementation() override;

	void PerformExit();

	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
		UFUNCTION()
		TArray<FString> GetMachineOptions() const;

		virtual void GetEmittedEvents(TSet<FName>& Events) const;
	#endif

private:

	#if WITH_EDITOR
		UFUNCTION()
		TArray<FString> GetSubMachineStateOptions() const;

		UFUNCTION()
		TArray<FString> GetStateEventOptions() const { return {}; }
	#endif
};
