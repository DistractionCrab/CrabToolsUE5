// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateMachine/ProcStateMachine.h"
#include "DialogueNode.generated.h"

USTRUCT(BlueprintType)
struct FDialogueData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "DialogueNode", meta = (AllowPrivateAccess = "true"))
	FText DialogueText;

	UPROPERTY(EditAnywhere, Category = "DialogueNode", meta = (AllowPrivateAccess = "true"))
	FName NextState;
};

/**
 * State Machine Node that is a composite of other nodes.
 */
UCLASS(Blueprintable, CollapseCategories)
class CRABTOOLSUE5_API UDialogueNode : public UStateNode
{
	GENERATED_BODY()
	
	/* The map of choices text to the next state. */
	UPROPERTY(EditAnywhere, Category = "DialogueNode", meta = (AllowPrivateAccess = "true"))
	TArray<FDialogueData> Choices;
public:

	
	//virtual void Initialize_Implementation(UProcStateMachine* POwner) override;
	//virtual void Tick_Implementation(float DeltaTime) override;
	//virtual void Event_Implementation(FName Event) override;
	//virtual void EventWithData_Implementation(FName EName, UObject* Data) override;
	//virtual void Enter_Implementation() override;
	//virtual void EnterWithData_Implementation(UObject* Data) override;
	//virtual void Exit_Implementation() override;	
	//virtual void ExitWithData_Implementation(UObject* Data) override;
};
