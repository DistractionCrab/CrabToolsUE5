#include "Kismet/StateMachineGraphPanelPinFactory.h"
#include "Kismet/K2Node_EmitEventFromDataTable.h"
#include "Kismet/K2Node_EmitEventWithDataFromDataTable.h"
#include "K2Node_CallFunction.h"
#include "SGraphPinDataTableRowName.h"
#include "SGraphPinNameList.h"

TSharedPtr<class SGraphPin> FStateMachineGraphPanelPinFactory::CreatePin(class UEdGraphPin* InPin) const
{
	if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Name)
	{
		UObject* Outer = InPin->GetOuter();

		// Create drop down combo boxes for DataTable and CurveTable RowName pins
		const UEdGraphPin* DataTablePin = nullptr;
		if (Outer->IsA(UK2Node_CallFunction::StaticClass()))
		{
			const UK2Node_CallFunction* CallFunctionNode = CastChecked<UK2Node_CallFunction>(Outer);
			if (CallFunctionNode)
			{
				const UFunction* FunctionToCall = CallFunctionNode->GetTargetFunction();
				if (FunctionToCall)
				{
					const FString& DataTablePinName = FunctionToCall->GetMetaData(FBlueprintMetadata::MD_DataTablePin);
					DataTablePin = CallFunctionNode->FindPin(DataTablePinName);
				}
			}
		}
		else if (Outer->IsA(UK2Node_EmitEventFromDataTable::StaticClass()))
		{
			const UK2Node_EmitEventFromDataTable* GetDataTableRowNode = CastChecked<UK2Node_EmitEventFromDataTable>(Outer);
			DataTablePin = GetDataTableRowNode->GetDataTablePin();
		}
		else if (Outer->IsA(UK2Node_EmitEventWithDataFromDataTable::StaticClass()))
		{
			const UK2Node_EmitEventWithDataFromDataTable* GetDataTableRowNode = CastChecked<UK2Node_EmitEventWithDataFromDataTable>(Outer);
			DataTablePin = GetDataTableRowNode->GetDataTablePin();
		}

		if (DataTablePin)
		{
			if (DataTablePin->DefaultObject != nullptr && DataTablePin->LinkedTo.Num() == 0)
			{
				if (auto DataTable = Cast<UDataTable>(DataTablePin->DefaultObject))
				{
					return SNew(SGraphPinDataTableRowName, InPin, DataTable);
				}
			}
		}
	}

	return nullptr;
}