// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateMachine/ProcStateMachine.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "Utils/Enums.h"
#include "ProcStateMachineComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRABTOOLSUE5_API UProcStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "ProcStateMachine")
	TSubclassOf<UProcStateMachine> MachineClass;
	UPROPERTY(VisibleAnywhere, Category = "ProcStateMachine")
	TObjectPtr<UProcStateMachine> Machine;
	// Cache of State Change Listeners to add to the machine when it is initiated.
	TArray<FStateChangeDispatcher> StateChangeListenerCache;

public:	
	// Sets default values for this component's properties
	UProcStateMachineComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	void Event(FName EName);

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	void EventWithData(FName EName, UObject* Data);

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine", meta=(ExpandEnumAsExecs="Branches"))
	UStateNode* FindNode(FName NodeName, ESearchResult& Branches);

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine", meta = (ExpandEnumAsExecs = "Branches"))
	UStateNode* FindNodeByPath(const FString& Path, ESearchResult& Branches);

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	void StateChangeListen(const FStateChangeDispatcher& Callback);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ProcStateMachine")
	FName CurrentStateName();

private:
	bool HasMachine();
};
