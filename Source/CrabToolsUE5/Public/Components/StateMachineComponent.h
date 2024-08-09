// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateMachine/StateMachine.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Utils/Enums.h"
#include "StateMachine/EventListener.h"
#include "StateMachineComponent.generated.h"


UCLASS( ClassGroup=(General), meta=(BlueprintSpawnableComponent) )
class CRABTOOLSUE5_API UStateMachineComponent : public UActorComponent, public IEventListenerInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "StateMachine")
	TSubclassOf<UStateMachine> MachineClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "StateMachine", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStateMachine> Machine;

	// Cache of State Change Listeners to add to the machine when it is initiated.
	TArray<FStateChangeDispatcher> StateChangeListenerCache;

public:	
	// Sets default values for this component's properties
	UStateMachineComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(
		float DeltaTime, 
		ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction)
		override;

	//UFUNCTION(BlueprintCallable, Category = "StateMachine")
	//void Event(FName EName);
	virtual void Event_Implementation(FName EName) override final { this->Event_Direct(EName); }
	void Event_Direct(FName EName);

	//UFUNCTION(BlueprintCallable, Category = "StateMachine")
	//void EventWithData(FName EName, UObject* Data);
	void EventWithData_Implementation(FName EName, UObject* Data) override final { this->EventWithData_Direct(EName, Data); }
	void EventWithData_Direct(FName EName, UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta=(ExpandEnumAsExecs="Branches"))
	UStateNode* FindNode(FName NodeName, ESearchResult& Branches);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void StateChangeListen(const FStateChangeDispatcher& Callback);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	FName CurrentStateName();

private:
	bool HasMachine();
};
