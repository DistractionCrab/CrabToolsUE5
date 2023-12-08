#include "Components/LockOnCamera/LockOnCamera.h"
#include "Components/LockOnCamera/LockOnInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

ULockOnCamera::ULockOnCamera(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	this->bForcePawnRotation = true;
	this->TargetArmLength = 400.0f;
	this->bUsePawnControlRotation = true; 

	// Create a follow camera
	this->FollowCamera = this->CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	this->FollowCamera->SetupAttachment(this, USpringArmComponent::SocketName); 
	this->FollowCamera->bUsePawnControlRotation = false;

	
}

void ULockOnCamera::BeginPlay() {
	Super::BeginPlay();
	APawn* POwner = Cast<APawn>(this->GetOwner());

	if (POwner) {
		this->PawnOwner = POwner;
	}
}

void ULockOnCamera::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (this->CurrentTarget.IsValid() && this->PawnOwner.IsValid()) {
		auto CharRot = this->GetDesiredRotation();
		this->PawnOwner->GetController()->SetControlRotation(CharRot);
	}
}

void ULockOnCamera::DetectCallback(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult) {

	this->AddTarget(OtherComp);
}

void ULockOnCamera::EndDetectCallback(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex) {

	this->RemoveTarget(OtherComp);
}

FRotator ULockOnCamera::GetDesiredRotation() const {
	if (this->CurrentTarget.IsValid()) {
		return FRotationMatrix::MakeFromX(this->CurrentTarget->GetComponentLocation() - this->GetComponentLocation()).Rotator();
	}
	else {
		return Super::GetDesiredRotation();
	}
	
}

void ULockOnCamera::LockOn() {
	if (this->DetectedComponents.Num() > 0) {
		this->bUsePawnControlRotation = false;
		if (this->PawnOwner.IsValid()) {		

			if (this->bForcePawnRotation) {
				this->PawnOwner->bUseControllerRotationYaw = true;
				if (ACharacter* c = Cast<ACharacter>(this->PawnOwner)) {
					c->GetCharacterMovement()->bOrientRotationToMovement = false;
				}			
			}
		}
		this->CurrentTarget = this->DetectedComponents[0];
	}
}

void ULockOnCamera::LockOff() {
	if (this->PawnOwner.IsValid()) {
		this->PawnOwner->bUseControllerRotationYaw = false;

		if (ACharacter* c = Cast<ACharacter>(this->PawnOwner)) {
			c->GetCharacterMovement()->bOrientRotationToMovement = true;
		}
	}
	this->bUsePawnControlRotation = true;
	this->CurrentTarget = nullptr;
}

void ULockOnCamera::Toggle() {
	if (this->IsLocked()) {
		this->LockOff();
	}
	else {
		this->LockOn();
	}
}

void ULockOnCamera::AddTarget(USceneComponent* Target) {
	this->DetectedComponents.Add(Target);
}

void ULockOnCamera::RemoveTarget(USceneComponent* Target) {
	this->DetectedComponents.Remove(Target);

	if (this->CurrentTarget == Target) {
		this->LockOff();
	}
}

bool ULockOnCamera::IsLocked() {
	return this->CurrentTarget.IsValid();
}

AActor* ULockOnCamera::GetLockedActor() {
	if (this->CurrentTarget.IsValid()) {
		return this->CurrentTarget->GetOwner();
	}
	else {
		return nullptr;
	}
}

USceneComponent* ULockOnCamera::GetLockedComponent() {
	return this->CurrentTarget.Get();
}

FRotator ULockOnCamera::GetLookRotation() {
	if (this->CurrentTarget.IsValid()) {
		return FRotationMatrix::MakeFromX(this->CurrentTarget->GetComponentLocation() - this->GetComponentLocation()).Rotator();
	}
	else {
		return FRotator::ZeroRotator;
	}
	
}