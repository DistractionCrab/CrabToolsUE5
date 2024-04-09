// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/ProcStateMachineComponent.h"
#include "SMPawn.generated.h"


UCLASS(config=Game)
class ASMPawn : public APawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UProcStateMachineComponent* StateMachineComponent;

public:
	ASMPawn();

};

