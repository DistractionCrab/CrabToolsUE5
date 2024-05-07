// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateMachine.h"
#include "ArrayNode.generated.h"

/**
 * State Machine Node that is a composite of other nodes.
 */
UCLASS(Blueprintable, CollapseCategories)
class CRABTOOLSUE5_API UArrayNode : public UStateNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Instanced, Category = "ArrayNode", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UStateNode*>> Nodes;

public:

	virtual void Initialize_Implementation(UStateMachine* POwner) override;
	virtual void Tick_Implementation(float DeltaTime) override;
	virtual void Event_Implementation(FName Event) override;
	virtual void EventWithData_Implementation(FName EName, UObject* Data) override;
	virtual void Enter_Implementation() override;
	virtual void EnterWithData_Implementation(UObject* Data) override;
	virtual void Exit_Implementation() override;	
	virtual void ExitWithData_Implementation(UObject* Data) override;
	virtual UStateNode* FindNodeByArray_Implementation(const TArray<FString>& Path, ESearchResult& Branches) override;
	virtual UStateNode* Substitute(FName SlotName, UStateNode* Node) override;
	virtual UStateNode* ExtractAs(TSubclassOf<UStateNode> Class) override;
};
