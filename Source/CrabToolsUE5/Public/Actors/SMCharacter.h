// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/StateMachineComponent.h"
#include "SMCharacter.generated.h"


UCLASS(config=Game)
class ASMCharacter : public ACharacter
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UStateMachineComponent* StateMachineComponent;

public:
	ASMCharacter();

};

