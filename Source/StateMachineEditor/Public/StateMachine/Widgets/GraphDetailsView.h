#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Misc/NotifyHook.h"

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SExpanderArrow.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/STreeView.h"
#include "SGraphActionMenu.h"

#include "StateMachine/Editor.h"

class IDetailsView;
class SGraphActionMenu;

/**
 * The tab which shows an overview of the State Machine graph, such as
 * Edges, Nodes, Events, or Aliases.
 */
class STATEMACHINEEDITOR_API SGraphDetailsView 
: public SCompoundWidget, public FNotifyHook, public FGCObject
{
private:
	//TSharedPtr<class SVerticalBox> StateListWidget;
	//TSharedPtr<class SVerticalBox> EventListWidget;
	//TSharedPtr<class SVerticalBox> AliasListWidget;

	//TSharedPtr<STreeView<TSharedPtr<FGraphActionNode>>> TreeView;
	TSharedPtr<SGraphActionMenu> GraphActionMenu;

public:
	SLATE_BEGIN_ARGS(SGraphDetailsView){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedPtr<class FEditor> InEditor);

	virtual FString GetReferencerName() const override
	{
		return TEXT("SGraphDetailsView");
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	
private:
	void BindEvents(TSharedPtr<class FEditor> InEditor);
	void OnGraphChanged(const FEdGraphEditAction& Action);
	void AddState(const class UEdStateNode* Node);

	// Section Functions.
	FText OnGetSectionTitle(int32 InSectionID);
	TSharedRef<SWidget> OnGetSectionWidget(TSharedRef<SWidget> RowWidget, int32 InSectionID);
};