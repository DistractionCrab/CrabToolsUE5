#include "Components/RPGSystem/RPGComponent.h"


#pragma region Component Code
void URPGComponent::BeginPlay() {
	Super::BeginPlay();

	for (auto& IntAtt : this->IntAttributes) {
		IntAtt.Value.Initialize(this);
	}

	for (auto& FloatAtt : this->FloatAttributes) {
		FloatAtt.Value.Initialize(this);
	}
}

TArray<FString> URPGComponent::GetIntAttributeNames() const {
	TArray<FString> Names;

	for (auto& Name : this->IntAttributes) {
		Names.Add(Name.Key.ToString());
	
	}
	return Names;
}

TArray<FString> URPGComponent::GetFloatAttributeNames() const {
	TArray<FString> Names;

	for (auto& Name : this->FloatAttributes) {
		Names.Add(Name.Key.ToString());

	}
	return Names;
}

FIntAttribute* URPGComponent::GetIntAttribute(FName Key) const {
	return (FIntAttribute*) this->IntAttributes.Find(Key);
}

FFloatAttribute* URPGComponent::GetFloatAttribute(FName Key) const {
	return (FFloatAttribute*) this->FloatAttributes.Find(Key);
}

void URPGComponent::ObserveIntAttribute(FName Key, const FIntAttributeCallback& Callback) {
	if (this->IntAttributes.Contains(Key)) {
		this->IntAttributes[Key].OnChangedEvent.Add(Callback);
	}
}

void URPGComponent::ObserveFloatAttribute(FName Key, const FFloatAttributeCallback& Callback) {
	if (this->FloatAttributes.Contains(Key)) {
		this->FloatAttributes[Key].OnChangedEvent.Add(Callback);
	}
}

int URPGComponent::GetIntAttributeValue(FName Key) {
	if (this->IntAttributes.Contains(Key)) {
		return this->IntAttributes[Key].GetValue();
	}
	else {
		return 0;
	}
}

float URPGComponent::GetFloatAttributeValue(FName Key) {
	if (this->FloatAttributes.Contains(Key)) {
		return this->FloatAttributes[Key].GetValue();
	}
	else {
		return 0;
	}
}

void URPGComponent::SetIntAttributeValue(FName Key, int Value) {
	if (this->IntAttributes.Contains(Key)) {
		auto& Ref = this->IntAttributes[Key];
		Ref.SetBase(Value);
	}
}

void URPGComponent::SetFloatAttributeValue(FName Key, float Value) {
	if (this->FloatAttributes.Contains(Key)) {
		auto& Ref = this->FloatAttributes[Key];
		Ref.SetBase(Value);
	}
}

#pragma endregion

#pragma region Integer Attributes

int FIntAttribute::GetValue() {
	return this->CompValue;
}

int FIntAttribute::GetBase() {
	return this->BaseValue;
}

int FIntAttribute::GetMax() {
	if (this->MaxRef) {
		return this->MaxRef->GetValue();
	}
	else {
		return MAX_int32;
	}
}

int FIntAttribute::GetMin() {
	if (this->MinRef) {
		return this->MinRef->GetValue();
	}
	else {
		return MIN_int32;
	}
}

void FIntAttribute::Initialize(URPGComponent* Comp) {
	if (this->MinAttribute != NAME_None) {
		this->MinRef = Comp->GetIntAttribute(this->MinAttribute);
	}

	if (this->MaxAttribute != NAME_None) {
		this->MaxRef = Comp->GetIntAttribute(this->MaxAttribute);
	}

	this->Refresh();
}

void FIntAttribute::Operate(UObject* Operator) {
	if (Operator && Operator->GetClass()->ImplementsInterface(UIntOperatorInterface::StaticClass())) {
		this->Operators.Add(Operator);
		this->Refresh();
	}
}

void FIntAttribute::Refresh() {
	int Base = this->BaseValue;
	int Max = this->GetMax();
	int Min = this->GetMin();

	for (auto& Op : this->Operators) {
		Base = IIntOperatorInterface::Execute_Operate(Op.Get(), Base);
		Base = FMath::Clamp(Base, Min, Max);
	}

	this->CompValue = Base;

	this->OnChangedEvent.Broadcast(Base, Min, Max);
}

void FIntAttribute::SetBase(int Value) {
	this->BaseValue = FMath::Clamp(Value, this->GetMin(), this->GetMax());
	this->Refresh();
}

#pragma endregion

#pragma region Float Attributes

float FFloatAttribute::GetValue() {
	return this->CompValue;
}

float FFloatAttribute::GetBase() {
	return this->BaseValue;
}

float FFloatAttribute::GetMax() {
	if (this->MaxRef) {
		return this->MaxRef->GetValue();
	}
	else {
		return std::numeric_limits<float>::infinity();
	}
}

float FFloatAttribute::GetMin() {
	if (this->MinRef) {
		return this->MinRef->GetValue();
	}
	else {
		return -std::numeric_limits<float>::infinity();
	}
}

void FFloatAttribute::Initialize(URPGComponent* Comp) {
	if (this->MinAttribute != NAME_None) {
		this->MinRef = Comp->GetFloatAttribute(this->MinAttribute);
	}

	if (this->MaxAttribute != NAME_None) {
		this->MaxRef = Comp->GetFloatAttribute(this->MaxAttribute);
	}

	this->Refresh();
}

void FFloatAttribute::Operate(UObject* Operator) {
	if (Operator && Operator->GetClass()->ImplementsInterface(UFloatOperatorInterface::StaticClass())) {
		this->Operators.Add(Operator);
		this->Refresh();
	}
}

void FFloatAttribute::Refresh() {
	float Base = this->BaseValue;
	float Max = this->GetMax();
	float Min = this->GetMin();

	for (auto& Op : this->Operators) {
		Base = IFloatOperatorInterface::Execute_Operate(Op.Get(), Base);
		Base = FMath::Clamp(Base, Min, Max);
	}

	this->CompValue = Base;

	this->OnChangedEvent.Broadcast(Base, Min, Max);
}

void FFloatAttribute::SetBase(float Value) {
	this->BaseValue = FMath::Clamp(Value, this->GetMin(), this->GetMax());
	this->Refresh();
}

#pragma endregion