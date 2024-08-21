#pragma once

#include "UObject/WeakObjectPtrTemplates.h"
#include "StateMachine/DataStructures.h"
#include "EdEventObject.generated.h"

class UEdStateGraph;

class FEventObjectActionEvents
{
public:

	DECLARE_MULTICAST_DELEGATE_TwoParams(FEventRenamed, FName, FName)
	FEventRenamed OnEventRenamed;

	DECLARE_MULTICAST_DELEGATE(FEventRemoved)
	FEventRemoved OnEventRemoved;

};

UCLASS(MinimalAPI)
class UEdEventObject : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "StateMachineEditor")
	FName EventName;

	UPROPERTY(EditDefaultsOnly, Category = "StateMachineEditor", meta=(Multiline=true))
	FEventSetRow Data;

public:

	FEventObjectActionEvents Events;

public:

	UEdStateGraph* GetGraph() const;
	void SetName(FName Name);
	FName GetEventName() const { return this->EventName; }
	FEventSetRow GetDescription() const { return this->Data; }
	FName RenameEvent(FName NewName);
	void Inspect();
	void Delete();

	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
};