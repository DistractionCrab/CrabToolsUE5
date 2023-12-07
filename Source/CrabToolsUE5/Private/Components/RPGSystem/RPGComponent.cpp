#include "UObject/UnrealTypePrivate.h"
#include "Components/RPGSystem/RPGComponent.h"


#pragma region Component Code
URPGComponent::URPGComponent(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer) {
	this->bWantsInitializeComponent = true;
}

void URPGComponent::BeginPlay() {
	Super::BeginPlay();
}

void URPGComponent::InitializeComponent() {
	Super::InitializeComponent();

	for (TFieldIterator<FStructProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT) {
		FStructProperty* f = *FIT;

		if (f->Struct == FIntAttribute::StaticStruct()) {
			auto Value = f->ContainerPtrToValuePtr<FIntAttribute>(this);
			Value->SetOwner(this);
			Value->Initialize(this);
		}
		else if (f->Struct == FIntResource::StaticStruct()) {
			auto Value = f->ContainerPtrToValuePtr<FIntResource>(this);
			Value->SetOwner(this);
			Value->Initialize(this);
		}
		else if (f->Struct == FFloatAttribute::StaticStruct()) {
			auto Value = f->ContainerPtrToValuePtr<FFloatAttribute>(this);
			Value->SetOwner(this);
			Value->Initialize(this);
		}
		else if (f->Struct == FFloatResource::StaticStruct()) {
			auto Value = f->ContainerPtrToValuePtr<FFloatResource>(this);
			Value->SetOwner(this);
			Value->Initialize(this);
		}
	}
}

TArray<FString> URPGComponent::GetIntAttributeNames() const {
	TArray<FString> Names;
	for (TFieldIterator<FStructProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT) {
		FStructProperty* f = *FIT;

		if (f->Struct == FIntAttribute::StaticStruct()) {
			Names.Add(f->GetFName().ToString());
		}
	}

	return Names;
}

TArray<FString> URPGComponent::GetFloatAttributeNames() const {
	TArray<FString> Names;
	for (TFieldIterator<FStructProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT) {
		FStructProperty* f = *FIT;

		if (f->Struct == FFloatAttribute::StaticStruct()) {
			Names.Add(f->GetFName().ToString());
		}
	}

	return Names;
}

void URPGComponent::PostEditChangeProperty(struct FPropertyChangedEvent& e) {
	Super::PostEditChangeProperty(e);
	this->Validate();
}

void URPGComponent::PostLoad() {
	Super::PostLoad();
	this->Validate();
}

void URPGComponent::Validate() {
	for (TFieldIterator<FStructProperty> FIT(this->GetClass(), EFieldIteratorFlags::IncludeSuper); FIT; ++FIT) {
		FStructProperty* f = *FIT;

		if (f->Struct == FIntAttribute::StaticStruct()) {
			auto Value = f->ContainerPtrToValuePtr<FIntAttribute>(this);
			Value->SetOwner(this);
		}
		else if (f->Struct == FIntResource::StaticStruct()) {
			auto Value = f->ContainerPtrToValuePtr<FIntResource>(this);
			Value->SetOwner(this);
		}
		else if (f->Struct == FFloatAttribute::StaticStruct()) {
			auto Value = f->ContainerPtrToValuePtr<FFloatAttribute>(this);
			Value->SetOwner(this);
		}
		else if (f->Struct == FFloatResource::StaticStruct()) {
			auto Value = f->ContainerPtrToValuePtr<FFloatResource>(this);
			Value->SetOwner(this);
		}
	}
}


#pragma endregion

#pragma region Integer Attributes & Resources

void FIntAttribute::SetOwner(URPGComponent* UOwner) { 
	this->Owner = UOwner; 

	for (auto& Op : this->Operators) {
		if (Op) {
			Op->SetOwner(UOwner);
		}
	}
}

int FIntAttribute::GetValue() const {
	return this->CompValue;
}


void FIntAttribute::Operate(UIntOperator* Op) {
	Op->SetOwner(this->Owner);
	this->Operators.Add(Op);

	this->Refresh();
}

void FIntAttribute::UnOperate(UIntOperator* Op) {
	auto Removed = this->Operators.Remove(Op);

	if (Removed > 0) {
		Op->SetOwner(nullptr);
	}

	this->Refresh();
}

void FIntAttribute::Initialize(URPGComponent* UOwner) {
	this->SetOwner(UOwner);

	for (auto& Op : this->Operators) {
		if (Op) {
			Op->SetOwner(UOwner);
			Op->Initialize();
		}
	}

	
	this->Refresh();
}

void FIntAttribute::Refresh() {
	// Sort operators as necessary.
	this->Operators.Sort([](const UIntOperator& A, const UIntOperator& B) { 
		return A.GetPriority() > B.GetPriority(); 
	});

	this->CompValue = this->BaseValue;
	for (auto& Op : this->Operators) {
		if (Op) {
			this->CompValue = Op->Operate(this->CompValue);
		}
	}

	this->IntChangedEvent.Broadcast(this->CompValue);
}

void FIntResource::SetOwner(URPGComponent* UOwner) {
	this->Owner = UOwner;
}

void FIntResource::Initialize(URPGComponent* UOwner) {
	this->SetOwner(UOwner);
	FProperty* MaxProp = UOwner->GetClass()->FindPropertyByName(this->MaxAttribute);
	FProperty* MinProp = UOwner->GetClass()->FindPropertyByName(this->MinAttribute);

	if (FStructProperty* SProp = CastField<FStructProperty>(MaxProp)) {
		if (SProp->Struct == FIntAttribute::StaticStruct()) {
			this->MaxAttributeRef = SProp->ContainerPtrToValuePtr<FIntAttribute>(UOwner);
		}
	}

	if (FStructProperty* SProp = CastField<FStructProperty>(MinProp)) {
		if (SProp->Struct == FIntAttribute::StaticStruct()) {
			this->MinAttributeRef = SProp->ContainerPtrToValuePtr<FIntAttribute>(UOwner);
		}
	}
}

void FIntResource::SetValue(int UValue) {
	this->Value = UValue;
	this->Refresh();
}

void FIntResource::Refresh() {
	if (!this->MaxAttributeRef) {
		UE_LOG(LogTemp, Warning, TEXT("MaxAttriRef was null."));
	}

	if (!this->MinAttributeRef) {
		UE_LOG(LogTemp, Warning, TEXT("MinAttriRef was null."));
	}

	this->Value = FMath::Clamp(this->Value, this->GetMin(), this->GetMax());
	this->IntChangedEvent.Broadcast(this->Value);
}

int FIntResource::GetMax() {
	if (this->MaxAttributeRef) {
		return this->MaxAttributeRef->GetValue();
	}
	else {
		return MAX_int32;
	}
}

int FIntResource::GetMin() {
	if (this->MinAttributeRef) {
		return this->MinAttributeRef->GetValue();
	}
	else {
		return MIN_int32;
	}
}

#pragma endregion

#pragma region Float Attributes & Resources


void FFloatAttribute::SetOwner(URPGComponent* UOwner) {
	this->Owner = UOwner;

	for (auto& Op : this->Operators) {
		if (Op) {
			Op->SetOwner(UOwner);
		}
	}
}

float FFloatAttribute::GetValue() const {
	return this->CompValue;
}


void FFloatAttribute::Operate(UFloatOperator* Op) {
	Op->SetOwner(this->Owner);
	this->Operators.Add(Op);

	this->Refresh();
}

void FFloatAttribute::UnOperate(UFloatOperator* Op) {
	auto Removed = this->Operators.Remove(Op);

	if (Removed > 0) {
		Op->SetOwner(nullptr);
	}

	this->Refresh();
}

void FFloatAttribute::Initialize(URPGComponent* UOwner) {
	this->SetOwner(UOwner);

	for (auto& Op : this->Operators) {
		if (Op) {
			Op->SetOwner(UOwner);
			Op->Initialize();
		}
	}


	this->Refresh();
}

void FFloatAttribute::Refresh() {
	// Sort operators as necessary.
	this->Operators.Sort([](const UFloatOperator& A, const UFloatOperator& B) {
		return A.GetPriority() > B.GetPriority();
		});

	this->CompValue = this->BaseValue;
	for (auto& Op : this->Operators) {
		if (Op) {
			this->CompValue = Op->Operate(this->CompValue);
		}
	}

	this->FloatChangedEvent.Broadcast(this->CompValue);
}

void FFloatResource::SetOwner(URPGComponent* UOwner) {
	this->Owner = UOwner;
}

void FFloatResource::Initialize(URPGComponent* UOwner) {
	this->SetOwner(UOwner);
	FProperty* MaxProp = UOwner->GetClass()->FindPropertyByName(this->MaxAttribute);
	FProperty* MinProp = UOwner->GetClass()->FindPropertyByName(this->MinAttribute);

	if (FStructProperty* SProp = CastField<FStructProperty>(MaxProp)) {
		if (SProp->Struct == FIntAttribute::StaticStruct()) {
			this->MaxAttributeRef = SProp->ContainerPtrToValuePtr<FFloatAttribute>(UOwner);
		}
	}

	if (FStructProperty* SProp = CastField<FStructProperty>(MinProp)) {
		if (SProp->Struct == FIntAttribute::StaticStruct()) {
			this->MinAttributeRef = SProp->ContainerPtrToValuePtr<FFloatAttribute>(UOwner);
		}
	}
}

void FFloatResource::SetValue(float UValue) {
	this->Value = UValue;
	this->Refresh();
}

void FFloatResource::Refresh() {
	this->Value = FMath::Clamp(this->Value, this->GetMin(), this->GetMax());
	this->FloatChangedEvent.Broadcast(this->Value);
}

float FFloatResource::GetMax() {
	if (this->MaxAttributeRef) {
		return this->MaxAttributeRef->GetValue();
	}
	else {
		return std::numeric_limits<float>::infinity();
	}
}

float FFloatResource::GetMin() {
	if (this->MinAttributeRef) {
		return this->MinAttributeRef->GetValue();
	}
	else {
		return -std::numeric_limits<float>::infinity();
	}
}

#pragma endregion