// Copyright Epic Games, Inc. All Rights Reserved.

#include "Kismet/K2Node_EmitEventWithDataFromInterface.h"


#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Containers/EnumAsByte.h"
#include "Containers/Map.h"
#include "Containers/UnrealString.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_K2.h"
#include "EditorCategoryUtils.h"
#include "Engine/MemberReference.h"
#include "HAL/PlatformMath.h"
#include "Internationalization/Internationalization.h"
#include "K2Node_CallFunction.h"
#include "K2Node_IfThenElse.h"
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
#include "StateMachine/StateMachineInterface.h"
#include "StateMachine/HelperLibrary.h"

class UBlueprint;

#define LOCTEXT_NAMESPACE "K2Node_EmitEventWithDataFromInterface"

namespace EmitEventWithDataFromInterfaceHelper
{
	const FName StateMachinePinName = "StateMachine";
	const FName InterfacePinName = "Interface";
	const FName EventPinName = "Event";
	const FName DataPinName = "Data";
}

UK2Node_EmitEventWithDataFromInterface::UK2Node_EmitEventWithDataFromInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeTooltip = LOCTEXT("NodeTooltip", "Emits an event to a State Machine from a Data Table.");
}

void UK2Node_EmitEventWithDataFromInterface::AllocateDefaultPins()
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	// Add execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	UEdGraphPin* RowFoundPin = CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);


	// Add State Machine Pin
	UEdGraphPin* StateMachinePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UEventListenerInterface::StaticClass(), EmitEventWithDataFromInterfaceHelper::StateMachinePinName);
	SetPinToolTip(*StateMachinePin, LOCTEXT("StateMachinePinDescription", "The EventListener to want to send an event to"));

	// Add Interface pin
	UEdGraphPin* InterfacePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UStateMachineInterface::StaticClass(), EmitEventWithDataFromInterfaceHelper::InterfacePinName);
	SetPinToolTip(*InterfacePin, LOCTEXT("InterfacePinDescription", "The Interface you want to retreive a row from"));

	// Row Name pin
	UEdGraphPin* EventPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, EmitEventWithDataFromInterfaceHelper::EventPinName);
	SetPinToolTip(*EventPin, LOCTEXT("EventPinDescription", "The name of the row to retrieve from the Interface"));

	// Add Data Pin
	UEdGraphPin* DataPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UObject::StaticClass(), EmitEventWithDataFromInterfaceHelper::DataPinName);
	SetPinToolTip(*DataPin, LOCTEXT("DataPinDescription", "The data to be passed with the event."));

	Super::AllocateDefaultPins();
}

void UK2Node_EmitEventWithDataFromInterface::SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const
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

void UK2Node_EmitEventWithDataFromInterface::RefreshEventOptions()
{
	// When the Interface pin gets a new value assigned, we need to update the Slate UI so that SGraphNodeCallParameterCollectionFunction will update the ParameterName drop down
	UEdGraph* Graph = GetGraph();
	Graph->NotifyNodeChanged(this);

	if (auto SMI = Cast<UStateMachineInterface>(this->GetInterfacePin()->DefaultObject))
	{
		this->OnInterfaceChanged.Broadcast(SMI);
	}
}

void UK2Node_EmitEventWithDataFromInterface::OnInterfaceRowListChanged(const UStateMachineInterface* Interface)
{
	UEdGraphPin* InterfacePin = GetInterfacePin();
	if (Interface && InterfacePin && Interface == InterfacePin->DefaultObject)
	{
		UEdGraphPin* EventPin = GetEventPin();
		const bool TryRefresh = EventPin && !EventPin->LinkedTo.Num();
		const FName CurrentName = EventPin ? FName(*EventPin->GetDefaultAsString()) : NAME_None;
		if (TryRefresh && EventPin && !Interface->GetEvents().Contains(CurrentName))
		{
			if (UBlueprint* BP = GetBlueprint())
			{
				FBlueprintEditorUtils::MarkBlueprintAsModified(BP);
			}
		}
	}
}

void UK2Node_EmitEventWithDataFromInterface::ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) 
{
	Super::ReallocatePinsDuringReconstruction(OldPins);

	if (UEdGraphPin* InterfacePin = GetInterfacePin(&OldPins))
	{
		if (UStateMachineInterface* Interface = Cast<UStateMachineInterface>(InterfacePin->DefaultObject))
		{
			PreloadObject(Interface);
		}
	}
}

void UK2Node_EmitEventWithDataFromInterface::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
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

FText UK2Node_EmitEventWithDataFromInterface::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::Utilities);
}

bool UK2Node_EmitEventWithDataFromInterface::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const
{
	if (MyPin == this->GetInterfacePin())
	{
		OutReason = "Cannot link to this pin: Choose a data table asset.";
		return true;
	}

	return false;
}

void UK2Node_EmitEventWithDataFromInterface::PinDefaultValueChanged(UEdGraphPin* ChangedPin) 
{
	if (ChangedPin && ChangedPin->PinName == EmitEventWithDataFromInterfaceHelper::InterfacePinName)
	{
		//RefreshOutputPinType();

		UEdGraphPin* EventPin = GetEventPin();
		UStateMachineInterface* Interface = Cast<UStateMachineInterface>(ChangedPin->DefaultObject);
		if (EventPin)
		{
			RefreshEventOptions();
		}
	}
}

FText UK2Node_EmitEventWithDataFromInterface::GetTooltipText() const
{
	return NodeTooltip;
}

UEdGraphPin* UK2Node_EmitEventWithDataFromInterface::GetThenPin()const
{
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UK2Node_EmitEventWithDataFromInterface::GetInterfacePin(const TArray<UEdGraphPin*>* InPinsToSearch /*= NULL*/) const
{
	const TArray<UEdGraphPin*>* PinsToSearch = InPinsToSearch ? InPinsToSearch : &Pins;
    
	UEdGraphPin* Pin = nullptr;
	for (UEdGraphPin* TestPin : *PinsToSearch)
	{
		if (TestPin && TestPin->PinName == EmitEventWithDataFromInterfaceHelper::InterfacePinName)
		{
			Pin = TestPin;
			break;
		}
	}
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_EmitEventWithDataFromInterface::GetEventPin() const
{
	UEdGraphPin* Pin = FindPinChecked(EmitEventWithDataFromInterfaceHelper::EventPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_EmitEventWithDataFromInterface::GetStateMachinePin() const
{
	UEdGraphPin* Pin = FindPinChecked(EmitEventWithDataFromInterfaceHelper::StateMachinePinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_EmitEventWithDataFromInterface::GetDataPin() const
{
	UEdGraphPin* Pin = FindPinChecked(EmitEventWithDataFromInterfaceHelper::DataPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

FText UK2Node_EmitEventWithDataFromInterface::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (TitleType == ENodeTitleType::MenuTitle)
	{
		return LOCTEXT("ListViewTitle", "Emit Event With Data From Interface");
	}
	else
	{
		return NSLOCTEXT("K2Node", "Interface_Title", "Emit Event With Data From Interface");
	}
}

void UK2Node_EmitEventWithDataFromInterface::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
    Super::ExpandNode(CompilerContext, SourceGraph);
    
	UEdGraphPin* OriginalInterfaceInPin = GetInterfacePin();
	UEdGraphPin* OriginalStateMachineInPin = GetStateMachinePin();

    UStateMachineInterface* Table = (OriginalInterfaceInPin != NULL) ? Cast<UStateMachineInterface>(OriginalInterfaceInPin->DefaultObject) : NULL;
    if((nullptr == OriginalInterfaceInPin) || (0 == OriginalInterfaceInPin->LinkedTo.Num() && nullptr == Table))
    {
        CompilerContext.MessageLog.Error(*LOCTEXT("EmitEventFromInterfaceNoInterface_Error", "EmitEventFromInterface must have a Interface specified.").ToString(), this);
        // we break exec links so this is the only error we get
        BreakAllNodeLinks();
        return;
    }

	// FUNCTION NODE
	const FName FunctionName = GET_FUNCTION_NAME_CHECKED(UStateMachineHelperLibrary, EmitEventWithData);
	UK2Node_CallFunction* EmitEventFromInterfaceFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	EmitEventFromInterfaceFunction->FunctionReference.SetExternalMember(FunctionName, UStateMachineHelperLibrary::StaticClass());
	EmitEventFromInterfaceFunction->AllocateDefaultPins();
    CompilerContext.MovePinLinksToIntermediate(*GetExecPin(), *(EmitEventFromInterfaceFunction->GetExecPin()));

	// Connect the input of our EmitEventFromInterface to the Input of our Function pin
    UEdGraphPin* StateMachineInPin = EmitEventFromInterfaceFunction->FindPinChecked(TEXT("Obj"));

	if(OriginalStateMachineInPin->LinkedTo.Num() > 0)
	{
		// Copy the connection
		CompilerContext.MovePinLinksToIntermediate(*OriginalStateMachineInPin, *StateMachineInPin);
	}
	else
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("EmitEventWithDataFromInterfaceNoInterface_Error", "EmitEventWithDataFromInterface must have a StateMachine specified.").ToString(), this);
		// we break exec links so this is the only error we get
		BreakAllNodeLinks();
		return;
	}

	UEdGraphPin* EventInPin = EmitEventFromInterfaceFunction->FindPinChecked(TEXT("EName"));
	CompilerContext.MovePinLinksToIntermediate(*GetEventPin(), *EventInPin);

	UEdGraphPin* DataInPin = EmitEventFromInterfaceFunction->FindPinChecked(TEXT("Data"));
	CompilerContext.MovePinLinksToIntermediate(*GetDataPin(), *DataInPin);

	// Get some pins to work with
    UEdGraphPin* FunctionThenPin = EmitEventFromInterfaceFunction->GetThenPin();
                
    // Hook up outputs
    CompilerContext.MovePinLinksToIntermediate(*GetThenPin(), *FunctionThenPin);

	BreakAllNodeLinks();
}

FSlateIcon UK2Node_EmitEventWithDataFromInterface::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	static FSlateIcon Icon(FAppStyle::GetAppStyleSetName(), "Kismet.AllClasses.FunctionIcon");
	return Icon;
}

void UK2Node_EmitEventWithDataFromInterface::PostReconstructNode()
{
	Super::PostReconstructNode();
}

void UK2Node_EmitEventWithDataFromInterface::EarlyValidation(class FCompilerResultsLog& MessageLog) const
{
	Super::EarlyValidation(MessageLog);

	const UEdGraphPin* InterfacePin = GetInterfacePin();
	const UEdGraphPin* EventPin = GetEventPin();
	if (!InterfacePin || !EventPin)
	{
		MessageLog.Error(*LOCTEXT("MissingPins", "Missing pins in @@").ToString(), this);
		return;
	}

	if (InterfacePin->LinkedTo.Num() == 0)
	{
		const UStateMachineInterface* Interface = Cast<UStateMachineInterface>(InterfacePin->DefaultObject);
		if (!Interface)
		{
			MessageLog.Error(*LOCTEXT("NoInterface", "No Interface in @@").ToString(), this);
			return;
		}

		if (!EventPin->LinkedTo.Num())
		{
			const FName CurrentName = FName(*EventPin->GetDefaultAsString());
			if (!Interface->GetEvents().Contains(CurrentName))
			{
				const FString Msg = FText::Format(
					LOCTEXT("WrongEventFmt", "'{0}' row name is not stored in '{1}'. @@"),
					FText::FromString(CurrentName.ToString()),
					FText::FromString(GetFullNameSafe(Interface))
				).ToString();
				MessageLog.Error(*Msg, this);
				return;
			}
		}
	}	
}

void UK2Node_EmitEventWithDataFromInterface::PreloadRequiredAssets()
{
	if (UEdGraphPin* InterfacePin = GetInterfacePin())
	{
		if (UStateMachineInterface* Interface = Cast<UStateMachineInterface>(InterfacePin->DefaultObject))
		{
			// make sure to properly load the data-table object so that we can 
			// farm the "RowStruct" property from it (below, in EmitEventFromInterfaceStructType)
			PreloadObject(Interface);
		}
	}
	return Super::PreloadRequiredAssets();
}

void UK2Node_EmitEventWithDataFromInterface::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);

	if (Pin == GetInterfacePin())
	{
		const bool bConnectionAdded = Pin->LinkedTo.Num() > 0;
		if (bConnectionAdded)
		{
			// if a connection was made, then we may need to rid ourselves of the row dropdown
			RefreshEventOptions();
		}
	}
}

#undef LOCTEXT_NAMESPACE
