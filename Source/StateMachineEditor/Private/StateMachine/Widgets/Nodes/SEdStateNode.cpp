#include "StateMachine/Widgets/Nodes/SEdStateNode.h"
#include "StateMachine/Widgets/Nodes/SEdEventGraphPin.h"
#include "StateMachine/Colors/StateMachineColors.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "StateMachine/EdGraph/EdAliasNode.h"

#define LOCTEXT_NAMESPACE "SEdStateNode"

constexpr const char* INVALID_STATE_NAME = "?Unknown State Name?";

void SEdStateNode::Construct(const FArguments& InArgs, UEdBaseStateNode* InNode) {
	this->GraphNode = InNode;
	this->UpdateGraphNode();
	
	InNode->Events.OnNameChanged.AddRaw(this, &SEdStateNode::OnNodeNameChanged);
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SEdStateNode::UpdateGraphNode()
{
	const FMargin NodePadding = FMargin(5);
	const FMargin NamePadding = FMargin(2);

	this->InputPins.Empty();
	this->OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	this->RightNodeBox.Reset();
	this->LeftNodeBox.Reset();

	const FSlateBrush* NodeTypeIcon = GetNameIcon();

	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);
	TSharedPtr<SErrorText> ErrorText;
	TSharedPtr<SVerticalBox> NodeBody;
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, this->GraphNode);
	
	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
				.BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))
				.Padding(0.0f)
				.BorderBackgroundColor(this, &SEdStateNode::GetBorderBackgroundColor)
				[
					SNew(SOverlay)

						+ SOverlay::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SVerticalBox)

								// Input Pin Area
								+ SVerticalBox::Slot()
								.FillHeight(1)
								[
									SAssignNew(LeftNodeBox, SVerticalBox)
								]

								// Output Pin Area	
								+ SVerticalBox::Slot()
								.FillHeight(1)
								[
									SAssignNew(RightNodeBox, SVerticalBox)
								]
						]

						+ SOverlay::Slot()
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.Padding(8.0f)
						[
							SNew(SBorder)
								.BorderImage(FAppStyle::GetBrush("Graph.StateNode.ColorSpill"))
								.BorderBackgroundColor(TitleShadowColor)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.Visibility(EVisibility::SelfHitTestInvisible)
								.Padding(6.0f)
								[
									SAssignNew(NodeBody, SVerticalBox)

										// Title
										+ SVerticalBox::Slot()
										.AutoHeight()
										[
											SNew(SHorizontalBox)

												// Error message
												+ SHorizontalBox::Slot()
												.AutoWidth()
												[
													SAssignNew(ErrorText, SErrorText)
														.BackgroundColor(this, &SEdStateNode::GetErrorColor)
														.ToolTipText(this, &SEdStateNode::GetErrorMsgToolTip)
												]

												// Icon
												+ SHorizontalBox::Slot()
												.AutoWidth()
												.VAlign(VAlign_Center)
												[
													SNew(SImage)
														.Image(NodeTypeIcon)
												]

												// Node Title
												+ SHorizontalBox::Slot()
												.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
												[
													SNew(SVerticalBox)
														+ SVerticalBox::Slot()
														.AutoHeight()
														[
															SAssignNew(InlineEditableText, SInlineEditableTextBlock)
																.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
																//.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
																.Text(FText::FromName(this->GetStateName()))
																.OnVerifyTextChanged(this, &SEdStateNode::OnVerifyNameTextChanged)
																.OnTextCommitted(this, &SEdStateNode::OnNameTextCommited)
																.IsReadOnly(this, &SEdStateNode::IsNameReadOnly)
																.IsSelected(this, &SEdStateNode::IsSelectedExclusively)
														]
														+ SVerticalBox::Slot()
														.AutoHeight()
														[
															NodeTitle.ToSharedRef()
														]
												]
										]
								]
						]
				]
		];

	this->CreatePinWidgets();
}

void SEdStateNode::CreatePinWidgets()
{
	UEdBaseStateNode* StateNode = CastChecked<UEdBaseStateNode>(GraphNode);

	for (int32 PinIdx = 0; PinIdx < StateNode->Pins.Num(); PinIdx++)
	{
		UEdGraphPin* MyPin = StateNode->Pins[PinIdx];
		if (!MyPin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SEdEventGraphPin, MyPin);

			AddPin(NewPin.ToSharedRef());
		}
	}
}

void SEdStateNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = PinObj && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility(TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced));
	}

	TSharedPtr<SVerticalBox> PinBox;
	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		PinBox = LeftNodeBox;
		InputPins.Add(PinToAdd);
	}
	else // Direction == EEdGraphPinDirection::EGPD_Output
	{
		PinBox = RightNodeBox;
		OutputPins.Add(PinToAdd);
	}

	if (PinBox)
	{
		PinBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillHeight(1.0f)
			//.Padding(6.0f, 0.0f)
			[
				PinToAdd
			];
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FSlateColor SEdStateNode::GetBorderBackgroundColor() const
{
	if (auto Node = Cast<UEdStateNode>(this->GetStateNode()))
	{
		if (Node->GetNodeType() == EStateNodeType::INLINE_NODE)
		{
			return StateMachineColors::NodeBorder::Root;
		}
		else if (Node->GetNodeType() == EStateNodeType::EXTENDED_NODE)
		{
			return StateMachineColors::NodeBorder::ExtensionProtected;
		}
		else if (Node->GetNodeType() == EStateNodeType::OVERRIDE_NODE)
		{
			return StateMachineColors::NodeBorder::ExtensionOverride;
		}
		else
		{
			return StateMachineColors::NodeBorder::Root;
		}
	}
	else if (auto AliasNode = Cast<UEdAliasNode>(this->GetStateNode()))
	{
		return StateMachineColors::NodeBorder::Alias;
	}
	else
	{
		return StateMachineColors::NodeBorder::Root;
	}	
}

const FSlateBrush* SEdStateNode::GetNameIcon() const
{
	return FAppStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Icon"));
}

bool SEdStateNode::IsNameReadOnly() const
{
	if (auto StateNode = Cast<UEdStateNode>(this->GetStateNode()))
	{
		if (StateNode->GetNodeType() == EStateNodeType::INLINE_NODE)
		{
			return false;
		}
	}
	else if (Cast<UEdAliasNode>(this->GetStateNode()))
	{
		return false;
	}

	return true;
}

void SEdStateNode::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	SGraphNode::OnNameTextCommited(InText, CommitInfo);

	if (this->GraphNode)
	{
		const FScopedTransaction Transaction(
			LOCTEXT(
				"StateMachineGraphEditorRenameNode", 
				"State Machine Graph Editor: Rename Node"));
		
		if (UEdBaseStateNode* CastNode = Cast<UEdBaseStateNode>(this->GraphNode))
		{
			CastNode->Modify();
			CastNode->RenameNode(FName(InText.ToString()));
		}
	}
}

FName SEdStateNode::GetStateName() const
{
	if (UEdBaseStateNode* Node = Cast<UEdBaseStateNode>(this->GraphNode))
	{
		return Node->GetNodeName();
	}

	return INVALID_STATE_NAME;
}

void SEdStateNode::OnNodeNameChanged(FName OldName, FName Name)
{
	this->UpdateGraphNode();
}

EVisibility SEdStateNode::GetDragOverMarkerVisibility() const
{
	return EVisibility::Visible;
}

FSlateColor SEdStateNode::GetBackgroundColor() const
{
	return StateMachineColors::NodeBody::Default;
	
}



#undef LOCTEXT_NAMESPACE