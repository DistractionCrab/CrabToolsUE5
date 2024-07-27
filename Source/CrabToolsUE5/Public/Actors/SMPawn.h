// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/StateMachineComponent.h"
#include "SMPawn.generated.h"


UCLASS(config=Game)
class ASMPawn : public APawn, public IEventListenerInterface
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UStateMachineComponent* StateMachineComponent;

public:
	ASMPawn();


	void Event_Implementation(FName EName) override { this->StateMachineComponent->Event(EName); }
	void EventWithData_Implementation(FName EName, UObject* Data) override
	{
		this->StateMachineComponent->EventWithData(EName, Data);
	}
};

