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
}


// Called every frame
void UPerspectiveManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	this->CurrentInterpolation += DeltaTime;
}

void UPerspectiveManager::ResetPerspective() { 
	this->PerspectiveCopy = nullptr;
	this->PerspectiveCopyComponent = nullptr;
}

bool UPerspectiveManager::IsBound() { 
	return this->PerspectiveCopy != nullptr || this->PerspectiveCopyComponent != nullptr;  
}

FRotator UPerspectiveManager::GetBasePerspective() {
	if (!this->DefaultPerspective.IsValid()) {
		AActor* Owner = this->GetOwner();

		if (Owner == nullptr) {
			return FRotator();
		}
		else {
			APawn* Check = Cast<APawn>(Owner);
			if (Check == nullptr) {
				return Owner->GetActorRotation();	
			} 
			else 
			{
				return Check->Controller->GetControlRotation();
			}
			
		}
	}
	else {
		return this->DefaultPerspective->GetActorRotation();
	}
}

FRotator UPerspectiveManager::GetPerspective() {
	
	if (this->PerspectiveCopyComponent.IsValid()) {
		FRotator Base = this->GetBasePerspective();
		FRotator Goal = this->PerspectiveCopyComponent->GetComponentRotation();

		if (this->InterpolationTime == 0.0f) {
			return Goal;
		}
		else {

			float Alpha = FMath::Min(this->CurrentInterpolation / this->InterpolationTime, 1.0);
			return FMath::Lerp(Base, Goal, Alpha);
		}		
	}
	else if (this->PerspectiveCopy.IsValid()) {
		FRotator Base = this->GetBasePerspective();
		FRotator Goal = this->PerspectiveCopy->GetActorRotation();

		if (this->InterpolationTime == 0.0f) {
			return Goal;
		}
		else {

			float Alpha = FMath::Min(this->CurrentInterpolation / this->InterpolationTime, 1.0);
			return FMath::Lerp(Base, Goal, Alpha);
		}		
	}
	else if (this->DefaultPerspective.IsValid()) {
		return this->DefaultPerspective->GetActorRotation();
	}
	else {
		AActor* Owner = this->GetOwner();

		if (Owner == nullptr) {
			return FRotator();
		}
		else {
			return Owner->GetActorRotation();
		}
	}
}