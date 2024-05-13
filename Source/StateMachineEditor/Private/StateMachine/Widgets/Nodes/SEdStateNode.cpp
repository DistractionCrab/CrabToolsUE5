#include "StateMachine/Widgets/Nodes/SEdStateNode.h"
#include "StateMachine/Colors/GenericColors.h"

#include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "SEdStateNode"

void SEdStateNode::Construct(const FArguments& InArgs, UEdStateNode* InNode) {
	this->GraphNode = InNode;

	const FMargin NodePadding = FMargin(5);
	const FMargin NamePadding = FMargin(2);

	this->InputPins.Empty();
	this->OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	this->RightNodeBox.Reset();
	this->LeftNodeBox.Reset();

	const FSlateBrush *NodeTypeIcon = GetNameIcon();

	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);
	TSharedPtr<SErrorText> ErrorText;
	TSharedPtr<SVerticalBox> NodeBody;
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

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
							+SHorizontalBox::Slot()
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
									.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
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

	//InNode->SEdNode = this;
}

FSlateColor SEdStateNode::GetBorderBackgroundColor() const
{
	return GenericGraphColors::NodeBorder::HighlightAbortRange0;
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

	this->InlineEditableText->SetText(InText);

	const FScopedTransaction Transaction(
		LOCTEXT("StateMachineGraphEditorRenameNode", "State Machine Graph Editor: Rename Node"));
	this->GraphNode->Modify();
}

//bool SEdStateNode::OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage)
//{
//	//FName NameCheck(InText.ToString());
//}

#undef LOCTEXT_NAMESPACE