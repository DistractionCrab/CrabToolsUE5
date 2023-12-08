// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PerspectiveManager.h"

// Sets default values for this component's properties
UPerspectiveManager::UPerspectiveManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPerspectiveManager::BeginPlay()
{
	Super::BeginPlay();

	this->PawnOwner = Cast<APawn>(this->GetOwner());
	this->Target.SetSubtype<TWeakObjectPtr<AActor>>();
}


// Called every frame
void UPerspectiveManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	this->CurrentInterpolation += DeltaTime;

	if (this->InterpolationTime <= 0.0f || this->CurrentInterpolation >= this->InterpolationTime) {
		this->CurrentRotation = this->GetRotation();
	}
	else {

		float Alpha = FMath::Min(this->CurrentInterpolation / this->InterpolationTime, 1.0);
		this->CurrentRotation =  FMath::Lerp(this->OldRotation, this->GetRotation(), Alpha);
	}
}

void UPerspectiveManager::ResetPerspective() { 
	this->Target.SetSubtype<TWeakObjectPtr<AActor>>();
	this->Target.GetSubtype<TWeakObjectPtr<AActor>>() = nullptr;
}


FRotator UPerspectiveManager::GetPerspective() {	
	return this->CurrentRotation;
}

void UPerspectiveManager::SetPerspective(AActor* PCopy) {
	this->CurrentInterpolation = 0;
	this->OldRotation = this->CurrentRotation;
	this->Target.SetSubtype<TWeakObjectPtr<AActor>>();
	this->Target.GetSubtype<TWeakObjectPtr<AActor>>() = PCopy;
}

void UPerspectiveManager::SetPerspectiveComponent(USceneComponent* PCopy) {
	this->CurrentInterpolation = 0;
	this->OldRotation = this->CurrentRotation;
	this->Target.SetSubtype<TWeakObjectPtr<USceneComponent>>();
	this->Target.GetSubtype<TWeakObjectPtr<USceneComponent>>() = PCopy;
}

void UPerspectiveManager::InvertPerspective() {
	this->Invert = !this->Invert;
}

FVector UPerspectiveManager::GetSourcePosition() {
	if (this->PawnOwner) {
		return this->PawnOwner->GetActorLocation();
	}
	else if (AActor* Owner = this->GetOwner()) {
		return Owner->GetActorLocation();
	}
	else {
		return FVector::ZeroVector;
	}	
}

FVector UPerspectiveManager::GetTargetPosition() {
	if (this->Target.GetCurrentSubtypeIndex() == 0) {
		auto R = this->Target.GetSubtype<TWeakObjectPtr<AActor>>();

		if (R.IsValid()) {
			return R->GetActorLocation();
		}
		else {
			return FVector::ZeroVector;
		}
	}
	else {
		auto R = this->Target.GetSubtype<TWeakObjectPtr<USceneComponent>>();

		if (R.IsValid()) {
			return R->GetComponentLocation();
		}
		else {
			return FVector::ZeroVector;
		}
	}

}

FRotator UPerspectiveManager::GetRotation() {
	if (this->HasTarget()) {
		if (this->Invert) {
			return FRotationMatrix::MakeFromX(this->GetSourcePosition() - this->GetTargetPosition()).Rotator();
		}
		else {
			return FRotationMatrix::MakeFromX(this->GetTargetPosition() - this->GetSourcePosition()).Rotator();
		}		
	}
	else {
		return this->GetSourceRotation();
	}	
}

FRotator UPerspectiveManager::GetSourceRotation() {
	if (this->PawnOwner) {
		return this->PawnOwner->GetController()->GetControlRotation();
	}
	else {
		if (AActor* Owner = this->GetOwner()) {
			return Owner->GetActorRotation();
		}
		else {
			return FRotator::ZeroRotator;
		}		
	}
}

bool UPerspectiveManager::HasTarget() {
	if (this->Target.GetCurrentSubtypeIndex() == 0) {
		auto R = this->Target.GetSubtype<TWeakObjectPtr<AActor>>();
		return R.IsValid();
	}
	else {
		auto R = this->Target.GetSubtype<TWeakObjectPtr<USceneComponent>>();
		return R.IsValid();		
	}
}