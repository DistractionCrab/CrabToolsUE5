// Copyright Epic Games, Inc. All Rights Reserved.

#include "Kismet/K2Node_EmitEventFromDataTable.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Containers/EnumAsByte.h"
#include "Containers/Map.h"
#include "Containers/UnrealString.h"
#include "DataTableEditorUtils.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_K2.h"
#include "EditorCategoryUtils.h"
#include "Engine/DataTable.h"
#include "Engine/MemberReference.h"
#include "HAL/PlatformMath.h"
#include "Internationalization/Internationalization.h"
#include "K2Node_CallFunction.h"
#include "K2Node_IfThenElse.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/CompilerResultsLog.h"
#include "KismetCompiler.h"
#include "Math/Color.h"
#include "Misc/AssertionMacros.h"
#include "Styling/AppStyle.h"
#include "Templates/Casts.h"
#include "UObject/Class.h"
#include "UObject/NameTypes.h"
#include "UObject/Object.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectBaseUtility.h"
#include "UObject/UnrealNames.h"
#include "UObject/WeakObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"
#include "StateMachine/HelperLibrary.h"

class UBlueprint;

#define LOCTEXT_NAMESPACE "K2Node_EmitEventFromDataTable"

namespace EmitEventFromDataTableHelper
{
	const FName StateMachinePinName = "StateMachine";
	const FName DataTablePinName = "DataTable";
	const FName RowNamePinName = "RowName";
}

UK2Node_EmitEventFromDataTable::UK2Node_EmitEventFromDataTable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeTooltip = LOCTEXT("NodeTooltip", "Emits an event to a State Machine from a Data Table.");
}

void UK2Node_EmitEventFromDataTable::AllocateDefaultPins()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	// Add execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	UEdGraphPin* RowFoundPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	// Add State Machine Pin
	UEdGraphPin* StateMachinePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UEventListenerInterface::StaticClass(), EmitEventFromDataTableHelper::StateMachinePinName);
	SetPinToolTip(*StateMachinePin, LOCTEXT("StateMachinePinDescription", "The EventListener to want to send an event to"));

	// Add DataTable pin
	UEdGraphPin* DataTablePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UDataTable::StaticClass(), EmitEventFromDataTableHelper::DataTablePinName);
	SetPinToolTip(*DataTablePin, LOCTEXT("DataTablePinDescription", "The DataTable you want to retreive a row from"));

	// Row Name pin
	UEdGraphPin* RowNamePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, EmitEventFromDataTableHelper::RowNamePinName);
	SetPinToolTip(*RowNamePin, LOCTEXT("RowNamePinDescription", "The name of the row to retrieve from the DataTable"));

	Super::AllocateDefaultPins();
}

void UK2Node_EmitEventFromDataTable::SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const
{
	MutatablePin.PinToolTip = UEdGraphSchema_K2::TypeToText(MutatablePin.PinType).ToString();

	UEdGraphSchema_K2 const* const K2Schema = Cast<const UEdGraphSchema_K2>(GetSchema());
	if (K2Schema != nullptr)
	{
		MutatablePin.PinToolTip += TEXT(" ");
		MutatablePin.PinToolTip += K2Schema->GetPinDisplayName(&MutatablePin).ToString();
	}

	MutatablePin.PinToolTip += FString(TEXT("\n")) + PinDescription.ToString();
}

void UK2Node_EmitEventFromDataTable::RefreshRowNameOptions()
{
	// When the DataTable pin gets a new value assigned, we need to update the Slate UI so that SGraphNodeCallParameterCollectionFunction will update the ParameterName drop down
	UEdGraph* Graph = GetGraph();
	Graph->NotifyNodeChanged(this);
}

void UK2Node_EmitEventFromDataTable::OnDataTableRowListChanged(const UDataTable* DataTable)
{
	UEdGraphPin* DataTablePin = GetDataTablePin();
	if (DataTable && DataTablePin && DataTable == DataTablePin->DefaultObject)
	{
		UEdGraphPin* RowNamePin = GetRowNamePin();
		const bool TryRefresh = RowNamePin && !RowNamePin->LinkedTo.Num();
		const FName CurrentName = RowNamePin ? FName(*RowNamePin->GetDefaultAsString()) : NAME_None;
		if (TryRefresh && RowNamePin && !DataTable->GetRowNames().Contains(CurrentName))
		{
			if (UBlueprint* BP = GetBlueprint())
			{
				FBlueprintEditorUtils::MarkBlueprintAsModified(BP);
			}
		}
	}
}

void UK2Node_EmitEventFromDataTable::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) 
{
	Super::ReallocatePinsDuringReconstruction(OldPins);

	if (UEdGraphPin* DataTablePin = GetDataTablePin(&OldPins))
	{
		if (UDataTable* DataTable = Cast<UDataTable>(DataTablePin->DefaultObject))
		{
			// make sure to properly load the data-table object so that we can 
			// farm the "RowStruct" property from it (below, in EmitEventFromDataTableStructType)
			PreloadObject(DataTable);
		}
	}
}

void UK2Node_EmitEventFromDataTable::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	// actions get registered under specific object-keys; the idea is that 
	// actions might have to be updated (or deleted) if their object-key is  
	// mutated (or removed)... here we use the node's class (so if the node 
	// type disappears, then the action should go with it)
	UClass* ActionKey = GetClass();
	// to keep from needlessly instantiating a UBlueprintNodeSpawner, first   
	// check to make sure that the registrar is looking for actions of this type
	// (could be regenerating actions for a specific asset, and therefore the 
	// registrar would only accept actions corresponding to that asset)
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_EmitEventFromDataTable::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::Utilities);
}

bool UK2Node_EmitEventFromDataTable::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const
{
	if (MyPin == this->GetDataTablePin())
	{
		OutReason = "Cannot link to this pin: Choose a data table asset.";
		return true;
	}

	return false;
}

void UK2Node_EmitEventFromDataTable::PinDefaultValueChanged(UEdGraphPin* ChangedPin) 
{
	if (ChangedPin && ChangedPin->PinName == EmitEventFromDataTableHelper::DataTablePinName)
	{
		//RefreshOutputPinType();

		UEdGraphPin* RowNamePin = GetRowNamePin();
		UDataTable*  DataTable = Cast<UDataTable>(ChangedPin->DefaultObject);
		if (RowNamePin)
		{
			if (DataTable && (RowNamePin->DefaultValue.IsEmpty() || !DataTable->GetRowMap().Contains(*RowNamePin->DefaultValue)))
			{
				if (auto Iterator = DataTable->GetRowMap().CreateConstIterator())
				{
					RowNamePin->DefaultValue = Iterator.Key().ToString();
				}
			}	

			RefreshRowNameOptions();
		}
	}
}

FText UK2Node_EmitEventFromDataTable::GetTooltipText() const
{
	return NodeTooltip;
}

UEdGraphPin* UK2Node_EmitEventFromDataTable::GetThenPin()const
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_EmitEventFromDataTable::GetDataTablePin(const TArray<UEdGraphPin*>* InPinsToSearch /*= NULL*/) const
{
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;
    
	UEdGraphPin* Pin = nullptr;
	for (UEdGraphPin* TestPin : *PinsToSearch)
	{
		if (TestPin && TestPin->PinName == EmitEventFromDataTableHelper::DataTablePinName)
		{
			Pin = TestPin;
			break;
		}
	}
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_EmitEventFromDataTable::GetRowNamePin() const
{
	UEdGraphPin* Pin = FindPinChecked(EmitEventFromDataTableHelper::RowNamePinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_EmitEventFromDataTable::GetStateMachinePin() const
{
	UEdGraphPin* Pin = FindPinChecked(EmitEventFromDataTableHelper::StateMachinePinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

FText UK2Node_EmitEventFromDataTable::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::MenuTitle)
	{
		return LOCTEXT("ListViewTitle", "Emit Event From DataTable");
	}
	else if (UEdGraphPin* DataTablePin = GetDataTablePin())
	{
		if (DataTablePin->LinkedTo.Num() > 0)
		{
			return NSLOCTEXT("K2Node", "DataTable_Title_Unknown", "Emit Event From DataTable");
		}
		else if (DataTablePin->DefaultObject == nullptr)
		{
			return NSLOCTEXT("K2Node", "DataTable_Title_None", "Emit Event From DataTable Row NONE");
		}
		else if (CachedNodeTitle.IsOutOfDate(this))
		{
			FFormatNamedArguments Args;
			Args.Add(TEXT("DataTableName"), FText::FromString(DataTablePin->DefaultObject->GetName()));

			FText LocFormat = NSLOCTEXT("K2Node", "DataTable", "Emit Event From DataTable Row {DataTableName}");
			// FText::Format() is slow, so we cache this to save on performance
			CachedNodeTitle.SetCachedText(FText::Format(LocFormat, Args), this);
		}
	}
	else
	{
		return NSLOCTEXT("K2Node", "DataTable_Title_None", "Get Data Table Row NONE");
	}	
	return CachedNodeTitle;
}

void UK2Node_EmitEventFromDataTable::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);
    
	UEdGraphPin* OriginalDataTableInPin = GetDataTablePin();
	UEdGraphPin* OriginalStateMachineInPin = GetStateMachinePin();

    UDataTable* Table = (OriginalDataTableInPin != NULL) ? Cast<UDataTable>(OriginalDataTableInPin->DefaultObject) : NULL;
    if((nullptr == OriginalDataTableInPin) || (0 == OriginalDataTableInPin->LinkedTo.Num() && nullptr == Table))
    {
        CompilerContext.MessageLog.Error(*LOCTEXT("EmitEventFromDataTableNoDataTable_Error", "EmitEventFromDataTable must have a DataTable specified.").ToString(), this);
        // we break exec links so this is the only error we get
        BreakAllNodeLinks();
        return;
    }

	// FUNCTION NODE
	const FName FunctionName = GET_FUNCTION_NAME_CHECKED(UStateMachineHelperLibrary, EmitEvent);
	UK2Node_CallFunction* EmitEventFromDataTableFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	EmitEventFromDataTableFunction->FunctionReference.SetExternalMember(FunctionName, UStateMachineHelperLibrary::StaticClass());
	EmitEventFromDataTableFunction->AllocateDefaultPins();
    CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *(EmitEventFromDataTableFunction->GetExecPin()));

	// Connect the input of our EmitEventFromDataTable to the Input of our Function pin
    UEdGraphPin* StateMachineInPin = EmitEventFromDataTableFunction->FindPinChecked(TEXT("Obj"));

	if(OriginalStateMachineInPin->LinkedTo.Num() > 0)
	{
		// Copy the connection
		CompilerContext.MovePinLinksToIntermediate(*OriginalStateMachineInPin, *StateMachineInPin);
	}
	else
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("EmitEventFromDataTableNoDataTable_Error", "EmitEventFromDataTable must have a StateMachine specified.").ToString(), this);
		// we break exec links so this is the only error we get
		BreakAllNodeLinks();
		return;
	}

	UEdGraphPin* RowNameInPin = EmitEventFromDataTableFunction->FindPinChecked(TEXT("EName"));
	CompilerContext.MovePinLinksToIntermediate(*GetRowNamePin(), *RowNameInPin);

	// Get some pins to work with
    UEdGraphPin* FunctionThenPin = EmitEventFromDataTableFunction->GetThenPin();
                
    // Hook up outputs
    CompilerContext.MovePinLinksToIntermediate(*GetThenPin(), *FunctionThenPin);

	BreakAllNodeLinks();
}

FSlateIcon UK2Node_EmitEventFromDataTable::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	static FSlateIcon Icon(FAppStyle::GetAppStyleSetName(), "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_EmitEventFromDataTable::PostReconstructNode()
{
	Super::PostReconstructNode();
}

void UK2Node_EmitEventFromDataTable::EarlyValidation(class FCompilerResultsLog& MessageLog) const
{
	Super::EarlyValidation(MessageLog);

	const UEdGraphPin* DataTablePin = GetDataTablePin();
	const UEdGraphPin* RowNamePin = GetRowNamePin();
	if (!DataTablePin || !RowNamePin)
	{
		MessageLog.Error(*LOCTEXT("MissingPins", "Missing pins in @@").ToString(), this);
		return;
	}

	if (DataTablePin->LinkedTo.Num() == 0)
	{
		const UDataTable* DataTable = Cast<UDataTable>(DataTablePin->DefaultObject);
		if (!DataTable)
		{
			MessageLog.Error(*LOCTEXT("NoDataTable", "No DataTable in @@").ToString(), this);
			return;
		}

		if (!RowNamePin->LinkedTo.Num())
		{
			const FName CurrentName = FName(*RowNamePin->GetDefaultAsString());
			if (!DataTable->GetRowNames().Contains(CurrentName))
			{
				const FString Msg = FText::Format(
					LOCTEXT("WrongRowNameFmt", "'{0}' row name is not stored in '{1}'. @@"),
					FText::FromString(CurrentName.ToString()),
					FText::FromString(GetFullNameSafe(DataTable))
				).ToString();
				MessageLog.Error(*Msg, this);
				return;
			}
		}
	}	
}

void UK2Node_EmitEventFromDataTable::PreloadRequiredAssets()
{
	if (UEdGraphPin* DataTablePin = GetDataTablePin())
	{
		if (UDataTable* DataTable = Cast<UDataTable>(DataTablePin->DefaultObject))
		{
			// make sure to properly load the data-table object so that we can 
			// farm the "RowStruct" property from it (below, in EmitEventFromDataTableStructType)
			PreloadObject(DataTable);
		}
	}
	return Super::PreloadRequiredAssets();
}

void UK2Node_EmitEventFromDataTable::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);

	if (Pin == GetDataTablePin())
	{
		const bool bConnectionAdded = Pin->LinkedTo.Num() > 0;
		if (bConnectionAdded)
		{
			// if a connection was made, then we may need to rid ourselves of the row dropdown
			RefreshRowNameOptions();
			// if the output connection was previously, incompatible, it now becomes the authority on this node's output type
			//RefreshOutputPinType();
		}
	}
}

#undef LOCTEXT_NAMESPACE
