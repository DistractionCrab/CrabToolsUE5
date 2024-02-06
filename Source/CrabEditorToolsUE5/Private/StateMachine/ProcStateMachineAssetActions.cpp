#include "StateMachine/ProcStateMachineAssetActions.h"
#include "StateMachine/ProcStateMachine.h"
 
UClass* FProcStateMachineAssetActions::GetSupportedClass() const
{
    return UProcStateMachine::StaticClass();
}
 
FText FProcStateMachineAssetActions::GetName() const
{
    return INVTEXT("Procedural State Machine");
}
 
FColor FProcStateMachineAssetActions::GetTypeColor() const
{
    return FColor::Cyan;
}
 
uint32 FProcStateMachineAssetActions::GetCategories()
{
    return EAssetTypeCategories::Misc;
}