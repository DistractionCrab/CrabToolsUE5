// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/ProcStateMachineComponent.h"
#include "SMCharacter.generated.h"


UCLASS(config=Game)
class ASMCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UProcStateMachineComponent* StateMachineComponent;

public:
	ASMCharacter();

};

