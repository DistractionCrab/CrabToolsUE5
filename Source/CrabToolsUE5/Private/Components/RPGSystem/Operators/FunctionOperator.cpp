#include "Components/RPGSystem/Operators/FunctionOperator.h"

int UIntFunctionOperator::Operate_Implementation(int Value) {
	if (this->Callback.IsBound()) {
		return this->Callback.Execute(Value);
	} else {
		return Value;
	}
}

void UIntFunctionOperator::Initialize_Implementation() {
	this->Callback.BindUFunction(this->GetOwner(), this->FunctionName);
}

TArray<FString> UIntFunctionOperator::GetIntOperatorFunctions() const {
	TArray<FString> Names;
	/*
	UE_LOG(LogTemp, Warning, TEXT("Looking for functions."));
	
	auto base = this->GetOwner()->FindFunction("IntPassThrough");
	for (TFieldIterator<UFunction> FIT(this->GetOwner()->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT) {
		UFunction* f = *FIT;

		if (f->IsSignatureCompatibleWith(base)) {
			Names.Add(f->GetName());
		}
	}
	*/
	return Names;
}