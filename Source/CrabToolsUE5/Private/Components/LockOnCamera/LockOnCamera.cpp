#include "Components/LockOnCamera/LockOnCamera.h"

ULockOnCamera::ULockOnCamera(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	this->bForcePawnRotation = false;
	this->TargetArmLength = 400.0f;
	this->bUsePawnControlRotation = true; 

	// Create a follow camera
	this->FollowCamera = this->CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	this->FollowCamera->SetupAttachment(this, USpringArmComponent::SocketName); 
	this->FollowCamera->bUsePawnControlRotation = false; 

	//FAttachmentTransformRules r(EAttachmentRule::KeepRelative, false);
	this->Detector = this->CreateDefaultSubobject<USphereComponent>(TEXT("Detector"));
	this->Detector->SetupAttachment(this, NAME_None);
	this->Detector->OnComponentBeginOverlap.AddDynamic(this, &ULockOnCamera::DetectCallback);
	this->Detector->OnComponentEndOverlap.AddDynamic(this, &ULockOnCamera::EndDetectCallback);

	this->Detector->SetCollisionProfileName("Custom");
	// Set the detector to ignore everything, will change to only overlap the specified channel later.
	this->Detector->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

}

void ULockOnCamera::BeginPlay() {
	APawn* POwner = Cast<APawn>(this->GetOwner());

	if (POwner) {
		this->PawnOwner = POwner;
	}
}

void ULockOnCamera::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (this->PawnOwner.IsValid() && this->bForcePawnRotation) {
		this->PawnOwner->GetController()->SetControlRotation(this->GetComponentRotation());
	}
}

void ULockOnCamera::DetectCallback(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult) {

	UE_LOG(LogTemp, Warning, TEXT("Detected lock on target %s"), *OtherActor->GetFName().ToString());
}

void ULockOnCamera::EndDetectCallback(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex) {

	UE_LOG(LogTemp, Warning, TEXT("Lost detection on target %s"), *OtherActor->GetFName().ToString());
}

FRotator ULockOnCamera::GetDesiredRotation() const {
	if (this->CurrentTarget.IsValid()) {
		return FRotationMatrix::MakeFromX(this->CurrentTarget->GetComponentLocation() - this->GetComponentScale()).Rotator();
	}
	else {
		return Super::GetDesiredRotation();
	}
	
}

void ULockOnCamera::LockOn() {

}