// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "LockOnCamera.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRABTOOLSUE5_API ULockOnCamera : public USpringArmComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Detector;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true", 
		ClampMin="0.0", ClampMax="180.0", UIMin="0.0", UIMax="180.0"))
	float LockOnAngle = 90;

	UPROPERTY(EditAnywhere, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	bool bForcePawnRotation;

	TWeakObjectPtr<APawn> PawnOwner;

	/* The array of components that can be locked onto. */
	TArray<TWeakObjectPtr<UPrimitiveComponent>> DetectedComponents;
	TWeakObjectPtr<UPrimitiveComponent> CurrentTarget;

public:	
	// Sets default values for this component's properties
	ULockOnCamera(const FObjectInitializer& ObjectInitializer);

protected:
	virtual FRotator GetDesiredRotation() const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void DetectCallback(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void EndDetectCallback(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category="Camera")
	void LockOn();

	
};
