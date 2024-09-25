#pragma once

#include "Containers/Array.h"
#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "HAL/PlatformCrt.h"
#include "Internationalization/Text.h"
#include "K2Node.h"
#include "Textures/SlateIcon.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"

#include "K2Node_EmitEventFromInterface.generated.h"

class FBlueprintActionDatabaseRegistrar;
class FString;
class UStateMachineInterface;
class UEdGraph;
class UEdGraphPin;
class UObject;
class UScriptStruct;
struct FLinearColor;

UCLASS()
class CRABNODES_API UK2Node_EmitEventFromInterface : public UK2Node
{
	GENERATED_UCLASS_BODY()

public:

	DECLARE_MULTICAST_DELEGATE_OneParam(FInterfaceChanged, UStateMachineInterface*)
	FInterfaceChanged OnInterfaceChanged;

public:

	//~ Begin UEdGraphNode Interface.
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual FText GetTooltipText() const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	virtual void PostReconstructNode() override;
	//~ End UEdGraphNode Interface.

	//~ Begin UK2Node Interface
	virtual bool IsNodeSafeToIgnore() const override { return true; }
	virtual void ReallocatePinsDuringReconstruction(TArray<UEdGraphPin*>& OldPins) override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;
	virtual bool IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const override;
	virtual void EarlyValidation(class FCompilerResultsLog& MessageLog) const override;
	virtual void PreloadRequiredAssets() override;
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	//~ End UK2Node Interface

	UEdGraphPin* GetStateMachinePin() const;
	/** Get the then output pin */
	UEdGraphPin* GetThenPin() const;
	/** Get the Data Table input pin */
	UEdGraphPin* GetInterfacePin(const TArray<UEdGraphPin*>* InPinsToSearch=NULL) const;
	/** Get the spawn transform input pin */	
	UEdGraphPin* GetEventPin() const;

	void OnInterfaceRowListChanged(const UStateMachineInterface* Interface);

private:
	/**
	 * Takes the specified "MutatablePin" and sets its 'PinToolTip' field (according
	 * to the specified description)
	 * 
	 * @param   MutatablePin	The pin you want to set tool-tip text on
	 * @param   PinDescription	A string describing the pin's purpose
	 */
	void SetPinToolTip(UEdGraphPin& MutatablePin, const FText& PinDescription) const;

	/** Triggers a refresh which will update the node's widget; aimed at updating the dropdown menu for the Event input */
	void RefreshEventOptions();

	/** Tooltip text for this node. */
	FText NodeTooltip;
};