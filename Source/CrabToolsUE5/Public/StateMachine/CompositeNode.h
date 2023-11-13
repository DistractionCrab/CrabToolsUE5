// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProcStateMachine.h"
#include "NodeTransition.h"
#include "CompositeNode.generated.h"

/**
 * State Machine Node that is a composite of other nodes.
 */
UCLASS(Blueprintable)
class CRABTOOLSUE5_API UCompositeNode : public UStateNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Instanced, Category = "CompositeNode", meta = (AllowPrivateAccess = "true"))
	TMap<FName, UStateNode*> Nodes;

public:

	virtual void Initialize_Implementation(UProcStateMachine* POwner) override;
	virtual void Tick_Implementation(float DeltaTime) override;
	virtual void Event_Implementation(FName Event) override;
	virtual void Enter_Implementation() override;
	virtual void Exit_Implementation() override;
	virtual UStateNode* FindNodeByArray_Implementation(const TArray<FString>& Path, ENodeSearchResult& Branches) override;
};
