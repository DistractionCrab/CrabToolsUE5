// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Components/LockOnCamera/LockOnInterface.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Camera/CameraComponent.h"
#include "LockOnAnchor.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRABTOOLSUE5_API ULockOnAnchor : public USphereComponent, public ILockOnInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="LockOnCamera", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* LockOnDisplay;

public:	
	// Sets default values for this component's properties
	ULockOnAnchor(const FObjectInitializer& ObjectInitializer);


	virtual void ActivateWidget_Implementation() override;
	virtual void DeactivateWidget_Implementation() override;
};
