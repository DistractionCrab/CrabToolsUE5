// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/StateMachineComponent.h"
#include "StateMachine/EventListener.h"
#include "SMCharacter.generated.h"


UCLASS(config=Game)
class ASMCharacter : public ACharacter, public IEventListenerInterface
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, 
		meta=(AllowPrivateAccess=true))
	class UStateMachineComponent* StateMachineComponent;

public:
	ASMCharacter();


	void Event_Implementation(FName EName) override { this->StateMachineComponent->Event(EName); }
	void EventWithData_Implementation(FName EName, UObject* Data) override 
	{
		this->StateMachineComponent->EventWithData(EName, Data);
	}
};

