#pragma once

#include "StateMachine/StateMachineEnum.h"
#include "StateMachine/Logging.h"
#include "IStateMachineLike.generated.h"

struct FSMPropertySearch;

struct FSMPropertyReference
{
    FProperty* PropertyRef = nullptr;
    TObjectPtr<UObject> StateMachine;

    template <class T> T* GetValue() const
    {
        if (this->PropertyRef && this->StateMachine)
        {
            return *this->PropertyRef->ContainerPtrToValuePtr<T*>(this->StateMachine);
        }

        return nullptr;
    }
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
    virtual FSMPropertyReference GetStateMachineProperty(FString& Address) const { return FSMPropertyReference(); }
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


struct CRABTOOLSUE5_API FSMPropertySearch
{

private:

    FFieldClass* FClass = nullptr;
    UScriptStruct* Struct = nullptr;
    UClass* Class = nullptr;

public:
    static FSMPropertySearch ObjectProperty(UClass* Class);
    static FSMPropertySearch StructProperty(UScriptStruct* Struct);
    static FSMPropertySearch Property(FFieldClass* FieldClass);

    bool Matches(FProperty* F) const;

    /* Used to get a property from a state machine at a given address, but with many checks. */
    template <class T> FSMPropertyReference GetProperty(IStateMachineLike* Machine, FString& Address)
    {
        FSMPropertyReference Ref = Machine->GetStateMachineProperty(Address);
        FProperty* Prop = Ref.PropertyRef;

        if (Prop)
        {
            if (!Prop->GetClass()->IsChildOf(this->FClass))
            {
                UE_LOG(LogStateMachine, Error, TEXT("Attempted get property of class %s, but found %s"),
                    *this->FClass->GetName(),
                    *Prop->GetClass()->GetName());
                return FSMPropertyReference();
            }

            if (Prop->GetClass()->IsChildOf(T::StaticClass()))
            {
                if (this->Class)
                {
                    FObjectProperty* ObjProp = (FObjectProperty*)Prop;

                    if (ObjProp->PropertyClass != this->Class)
                    {
                        UE_LOG(LogStateMachine, Error, TEXT("Attempted get object property of type %s, but found %s"),
                            *this->Class->GetName(),
                            *ObjProp->PropertyClass->GetName());

                        return FSMPropertyReference();
                    }
                }
                else if (this->Struct)
                {
                    FStructProperty* StructProp = (FStructProperty*)Prop;

                    if (StructProp->Struct != this->Struct)
                    {
                        UE_LOG(LogStateMachine, Error, TEXT("Attempted get struct property of type %s, but found %s"),
                            *this->Struct->GetName(),
                            *StructProp->Struct->GetName());

                        return FSMPropertyReference();
                    }
                }
            }
            else
            {
                UE_LOG(LogStateMachine, Error, TEXT("Attempted to perform invalid conversion: %s -> %s"),
                    *Prop->GetName(),
                    *T::StaticClass()->GetName());

                return FSMPropertyReference();
            }
        }
        else
        {
            return FSMPropertyReference();
        }

        return Ref;
    }
};