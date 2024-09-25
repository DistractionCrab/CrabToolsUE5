#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SGraphPinNameList.h"
#include "UObject/SoftObjectPtr.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class UStateMachineInterface;
class UEdGraphPin;

class CRABNODES_API SGraphPinSMEventName : public SGraphPinNameList
{
public:
	SLATE_BEGIN_ARGS(SGraphPinSMEventName) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj, UStateMachineInterface* InInterface);

	SGraphPinSMEventName();
	virtual ~SGraphPinSMEventName();

	void RefreshNameList(UStateMachineInterface* NewInterface);

};