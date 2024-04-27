// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateMachine/StateMachine.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Utils/Enums.h"
#include "StateMachineComponent.generated.h"


UCLASS( ClassGroup=(General), meta=(BlueprintSpawnableComponent) )
class CRABTOOLSUE5_API UStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "StateMachine")
	TSubclassOf<UStateMachine> MachineClass;

	UPROPERTY(EditAnywhere, Instanced, Category = "StateMachine")
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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void Event(FName EName);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void EventWithData(FName EName, UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta=(ExpandEnumAsExecs="Branches"))
	UStateNode* FindNode(FName NodeName, ESearchResult& Branches);

	UFUNCTION(BlueprintCallable, Category = "StateMachine", meta = (ExpandEnumAsExecs = "Branches"))
	UStateNode* FindNodeByPath(const FString& Path, ESearchResult& Branches);

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void StateChangeListen(const FStateChangeDispatcher& Callback);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StateMachine")
	FName CurrentStateName();

private:
	bool HasMachine();
};
