// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/IntVariable.h"

// Sets default values for this component's properties
UIntVariable::UIntVariable()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UIntVariable::BeginPlay()
{
	
}


// Called every frame
void UIntVariable::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	this->Value += this->TickAmount;
}



int UIntVariable::SetValue(int NewValue) {
	NewValue = FMath::Clamp(NewValue, this->Min, this->Max);
	for (FIntModifier& f : this->ChangeModifiers) {
		if (f.IsBound()) {
			NewValue = FMath::Clamp(f.Execute(this->Value, NewValue, this), this->Min, this->Min);
		}
	}

	this->ChangeObserversCollection.Broadcast(this->Value, NewValue, this);
	this->Value = NewValue;
	return this->Value;
}

int UIntVariable::OperateValue(UVariableOperator* Op) {
	for (FIntOperatorObserver& f : this->ChangeOperators) {
		f.ExecuteIfBound(this->Value, Op, this);
	}


	return this->SetValue(Op->ApplyInt(this->Value));
}