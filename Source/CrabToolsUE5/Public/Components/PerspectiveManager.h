// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include "PerspectiveManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRABTOOLSUE5_API UPerspectiveManager : public UActorComponent
{
	GENERATED_BODY()

	/* The amount of time it should take until interpolation from the old perspective to the new perspective should take.*/
	UPROPERTY(EditAnywhere, Category = "Perspective Manager", meta = (AllowPrivateAccess = "true"))
	float InterpolationTime;

	float CurrentInterpolation = 0;

	UPROPERTY(EditAnywhere, Category = "Perspective Manager", meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AActor> DefaultPerspective;

	UPROPERTY(EditAnywhere, Category = "Perspective Manager", meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AActor> PerspectiveCopy;

	UPROPERTY(EditAnywhere, Category = "Perspective Manager", meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<USceneComponent> PerspectiveCopyComponent;

public:	
	// Sets default values for this component's properties
	UPerspectiveManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FRotator GetPerspective();
	FRotator GetBasePerspective();

	UFUNCTION(BlueprintCallable, Category = "Perspective Manager")
	void SetPerspective(AActor* PCopy) { 
		this->PerspectiveCopy = PCopy; 
		this->CurrentInterpolation = 0;
	}

	UFUNCTION(BlueprintCallable, Category = "Perspective Manager")
	void SetPerspectiveComponent(USceneComponent* PCopy) { 
		this->PerspectiveCopyComponent = PCopy; 
		this->CurrentInterpolation = 0;
	}

	UFUNCTION(BlueprintCallable, Category = "Perspective Manager")
	void SetDefaultPerspective(AActor* PCopy) { this->DefaultPerspective = PCopy; }

	UFUNCTION(BlueprintCallable, Category = "Perspective Manager")
	void ResetPerspective();

	UFUNCTION(BlueprintCallable, Category = "Perspective Manager")
	bool IsBound();
};
