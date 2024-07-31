#include "StateMachine/Widgets/Nodes/SEdStateNode.h"
#include "StateMachine/Widgets/Nodes/SEdEventGraphPin.h"
#include "StateMachine/Colors/GenericColors.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "SEdStateNode"

void SEdStateNode::Construct(const FArguments& InArgs, UEdStateNode* InNode) {
	this->GraphNode = InNode;
	this->UpdateGraphNode();
	
	InNode->Events.OnNameChanged.AddRaw(this, &SEdStateNode::OnNodeNameChanged);
}

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
	//FText DisplayText = FText::FromName(InNode->GetStateName());


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

FSlateColor SEdStateNode::GetBorderBackgroundColor() const
{
	return GenericGraphColors::NodeBorder::Root;
}

const FSlateBrush* SEdStateNode::GetNameIcon() const
{
	return FAppStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Icon"));
}

bool SEdStateNode::IsNameReadOnly() const
{
	return false;
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
		
		if (UEdStateNode* CastNode = Cast<UEdStateNode>(this->GraphNode))
		{
			CastNode->SetStateName(FName(InText.ToString()));
		}

		this->GraphNode->Modify();
	}
}

FName SEdStateNode::GetStateName() const
{
	if (UEdStateNode* Node = Cast<UEdStateNode>(this->GraphNode))
	{
		return Node->GetStateName();
	}

	return NAME_None;
}

void SEdStateNode::OnNodeNameChanged(FName OldName, FName Name)
{
	this->InlineEditableText->SetText(FText::FromName(Name));
}

EVisibility SEdStateNode::GetDragOverMarkerVisibility() const
{
	return EVisibility::Visible;
}

FSlateColor SEdStateNode::GetBackgroundColor() const
{
	return GenericGraphColors::NodeBody::Default;
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

void SEdStateNode::CreatePinWidgets()
{
	UEdStateNode* StateNode = CastChecked<UEdStateNode>(GraphNode);

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


#undef LOCTEXT_NAMESPACE