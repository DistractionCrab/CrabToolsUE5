#pragma once

#include "IStateMachineLike.generated.h"

/* Interface for objects which look like State Machines data-wise, but not necessarily function-wise.*/
UINTERFACE(MinimalAPI)
class UStateMachineLike : public UInterface
{
    GENERATED_BODY()
};

class IStateMachineLike
{    
    GENERATED_BODY()

public:
    virtual TArray<FString> GetConditionOptions() const { return {}; };
    virtual TArray<FString> GetMachineOptions() const { return {}; };
    virtual TArray<FString> GetEventOptions() const { return {}; };
    virtual TArray<FString> GetStateOptions() const { return {}; };
    virtual TArray<FString> GetSubMachineStateOptions(FName SMName) const { return {}; };
};

/* Interface for objects that act like states within a state machine. */
UINTERFACE(MinimalAPI)
class UStateLike : public UInterface
{
    GENERATED_BODY()
};

class IStateLike
{
    GENERATED_BODY()

public:

    virtual TArray<FString> GetEventOptions() const { return {}; };
    virtual void RenameEvent(FName From, FName To) {}
    virtual void DeleteEvent(FName EventName) {}
};