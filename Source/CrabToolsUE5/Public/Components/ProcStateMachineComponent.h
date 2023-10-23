// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateMachine/ProcStateMachine.h"
#include "ProcStateMachineComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRABTOOLSUE5_API UProcStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Instanced, Category = "ProcStateMachine")
	UProcStateMachine* Machine;

public:	
	// Sets default values for this component's properties
	UProcStateMachineComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine")
	void Event(FName EName);

	UFUNCTION(BlueprintCallable, Category = "ProcStateMachine", meta=(ExpandEnumAsExecs="Branches"))
	UStateNode* FindNode(FName NodeName, ENodeSearchResult& Branches);
};
