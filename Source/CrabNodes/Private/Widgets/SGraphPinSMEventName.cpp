#include "Widgets/SGraphPinSMEventName.h"

#include "Containers/Array.h"
#include "Engine/DataTable.h"
#include "HAL/PlatformCrt.h"
#include "StateMachine/StateMachineInterface.h"
#include "Templates/SharedPointer.h"
#include "UObject/NameTypes.h"

class UEdGraphPin;

void SGraphPinSMEventName::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj, UStateMachineInterface* InInterface)
{
	RefreshNameList(InInterface);

	SGraphPinNameList::Construct(SGraphPinNameList::FArguments(), InGraphPinObj, NameList);
}

SGraphPinSMEventName::SGraphPinSMEventName()
{
}

SGraphPinSMEventName::~SGraphPinSMEventName()
{
}


void SGraphPinSMEventName::RefreshNameList(UStateMachineInterface* Interface)
{
	NameList.Empty();

	if (Interface)
	{		
		auto Names = Interface->GetEvents();
		for (auto& Name : Names)
		{
			TSharedPtr<FName> RowNameItem = MakeShareable(new FName(Name));
			NameList.Add(RowNameItem);
		}
	}
}
