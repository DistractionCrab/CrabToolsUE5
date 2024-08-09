// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/StateMachineComponent.h"
#include "Engine.h"

// Sets default values for this component's properties
UStateMachineComponent::UStateMachineComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}


// Called when the game starts
void UStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();
	if (this->MachineClass.Get() != nullptr) {
		if (this->Machine == nullptr) {
			this->Machine = NewObject<UStateMachine>(this, this->MachineClass);
		}		

		for (auto const& l : this->StateChangeListenerCache) {
			this->Machine->StateChangeListen(l);
		}

		this->StateChangeListenerCache.Empty();
	}

	if (this->Machine)
	{		
		this->Machine->Initialize_Internal(this->GetOwner());
	}
}


// Called every frame
void UStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (this->Machine != nullptr) {
		this->Machine->Tick(DeltaTime);
	}
}


void UStateMachineComponent::Event_Direct(FName EName) {
	if (this->HasMachine()) {
		this->Machine->Event_Direct(EName);
	}
}

void UStateMachineComponent::EventWithData_Direct(FName EName, UObject* Data) {
	if (this->HasMachine()) {
		this->Machine->EventWithData_Direct(EName, Data);
	}
}

UStateNode* UStateMachineComponent::FindNode(FName NodeName, ESearchResult& Branches) {
	if (this->HasMachine()) {
		return this->Machine->FindNode(NodeName, Branches);
	}
	else {
		Branches = ESearchResult::NotFound;
		return nullptr;
	}
}

void UStateMachineComponent::StateChangeListen(const FStateChangeDispatcher& Callback) {
	if (this->HasMachine()) {
		this->Machine->StateChangeListen(Callback);
	}
	else {
		this->StateChangeListenerCache.Add(Callback);
	}
}

FName UStateMachineComponent::CurrentStateName() {
	return this->Machine->GetCurrentStateName();
}

bool UStateMachineComponent::HasMachine() {
	return this->Machine != nullptr;
}
