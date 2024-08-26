#pragma once

#include "StateMachine/StateMachineEnum.h"
#include "IStateMachineLike.generated.h"

struct FSMPropertySearch
{
    FFieldClass* FClass = nullptr;
    UScriptStruct* Struct = nullptr;
    UClass* Class = nullptr;

    CRABTOOLSUE5_API bool Matches(FProperty* F) const;
};

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
    // Condition Getters
    virtual TArray<FString> GetConditionOptions() const { return {}; };
    virtual TArray<FString> GetDataConditionOptions() const { return {}; };
    
    // States and Events
    virtual TArray<FString> GetEventOptions() const { return {}; };
    virtual TArray<FString> GetStateOptions(const UObject* Asker) const { return {}; };
    virtual TArray<FName> GetEmittedEvents() const { return {}; };

    // SubMachine Getters
    virtual TArray<FString> GetMachineOptions() const { return {}; };
    virtual IStateMachineLike* GetSubMachine(FString& Address) const { return nullptr; }
    virtual TArray<FString> GetSubMachineStateOptions(FName SMName) const { return {}; };

    // Property Getters
    virtual TArray<FString> GetPropertiesOptions(FSMPropertySearch& SearchParam) const { return {}; }
    virtual FProperty* GetStateMachineProperty(FString& Address) const { return nullptr; }
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

    /* Returns the list of states that enter this state. */
    virtual TArray<FString> GetEnterStates() const { return {}; }

    /* Returns the list of states that this state can enter. */
    virtual TArray<FString> GetExitStates() const { return {}; }
};
