#pragma once

#include "UObject/WeakObjectPtrTemplates.h"
#include "EdEventObject.generated.h"

class FEventObjectActionEvents
{
public:

	DECLARE_MULTICAST_DELEGATE_OneParam(FEventRenamed, FName)
	FEventRenamed OnEventRenamed;

};

UCLASS(MinimalAPI)
class UEdEventObject : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "StateMachineEditor")
	bool bIsVariable = true;

	UPROPERTY(EditDefaultsOnly, Category = "StateMachineEditor")
	FName EventName;

public:

	FEventObjectActionEvents Events;

public:
	class UEdStateGraph* GetGraph() const;
	void SetName(FName Name);
	FName GetName() { return this->EventName; }

	FName RenameEvent(FName NewName);
};