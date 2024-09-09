#pragma once

#include "CoreMinimal.h"
#include "DataStructures.generated.h"

USTRUCT(BlueprintType)
struct FEventSetRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "EventSet",
		meta = (MultiLine = true))
	FText Description;
};


/* Structure used to store an event to be used by a node. */
USTRUCT(BlueprintType)
struct FEventSlot
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "StateMachine|Events",
		meta = (GetOptions = "GetEventOptions"))
	FName EventName;

	friend uint32 GetTypeHash(const FEventSlot& Value)
	{
		return GetTypeHash(Value.EventName);
	}
};

USTRUCT(BlueprintType)
struct FStateSlot
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "StateMachine|Events",
		meta = (GetOptions = "GetStateOptions"))
	FName StateName;

	friend uint32 GetTypeHash(const FStateSlot& Value)
	{
		return GetTypeHash(Value.StateName);
	}
};

USTRUCT(BlueprintType)
struct FIncomingStateSlot
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "StateMachine|Events",
		meta = (GetOptions = "GetIncomingStateOptions"))
	FName StateName;

	friend uint32 GetTypeHash(const FIncomingStateSlot& Value)
	{
		return GetTypeHash(Value.StateName);
	}
};

USTRUCT(BlueprintType)
struct FOutgoingStateSlot
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "StateMachine|Events",
		meta = (GetOptions = "GetOutgoingStateOptions"))
	FName StateName;

	friend uint32 GetTypeHash(const FOutgoingStateSlot& Value)
	{
		return GetTypeHash(Value.StateName);
	}
};

/* Structure used to store a reference to a submachine. */
USTRUCT(BlueprintType)
struct FSubMachineSlot
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "StateMachine",
		meta = (GetOptions = "GetMachineOptions"))
	FName MachineName;

	friend uint32 GetTypeHash(const FSubMachineSlot& Value)
	{
		return GetTypeHash(Value.MachineName);
	}
};