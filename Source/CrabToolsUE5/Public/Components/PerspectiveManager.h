// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "Containers/Union.h"
#include "PerspectiveManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRABTOOLSUE5_API UPerspectiveManager : public UActorComponent
{
	GENERATED_BODY()

	/* The amount of time it should take until interpolation from the old perspective to the new perspective should take.*/
	UPROPERTY(EditDefaultsOnly, Category = "PerspectiveManager", meta = (AllowPrivateAccess = "true"))
	float InterpolationTime;

	float CurrentInterpolation = 0;


	TUnion<TWeakObjectPtr<AActor>, TWeakObjectPtr<USceneComponent>> Target;
	APawn* PawnOwner;
	FRotator OldRotation;
	FRotator CurrentRotation;
	bool Invert = false;

public:	
	// Sets default values for this component's properties
	UPerspectiveManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PerspectiveManager")
	FRotator GetPerspective();

	UFUNCTION(BlueprintCallable, Category = "PerspectiveManager")
	void SetPerspective(AActor* PCopy);

	UFUNCTION(BlueprintCallable, Category = "PerspectiveManager")
	void SetPerspectiveComponent(USceneComponent* PCopy);

	UFUNCTION(BlueprintCallable, Category = "PerspectiveManager")
	void InvertPerspective();


	UFUNCTION(BlueprintCallable, Category = "PerspectiveManager")
	void ResetPerspective();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "PerspectiveManager")
	bool HasTarget();

	FRotator GetSourceRotation();
	FVector GetSourcePosition();
	FVector GetTargetPosition();
	FRotator GetRotation();

	
};
