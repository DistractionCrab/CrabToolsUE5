#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "Styling/SlateColor.h"
#include "Layout/SlateRect.h"
#include "Layout/Visibility.h"
#include "Layout/Geometry.h"
#include "Input/Reply.h"
#include "Layout/ArrangedWidget.h"
#include "Widgets/SWidget.h"
#include "UObject/GCObject.h"
#include "Types/SlateStructs.h"
#include "Animation/CurveSequence.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/SlateBrush.h"
#include "Components/Widget.h"
#include "StateMachine/Editor.h"
#include "Settings/LevelEditorPlaySettings.h"
#include "Layout/WidgetPath.h"
#include "GraphEditor.h"


class STATEMACHINEEDITOR_API SGraphView : public SCompoundWidget, public FGCObject
{

private:

	TWeakObjectPtr<UStateMachineBlueprint> BlueprintRef;
	TSharedPtr<SGraphEditor> GraphEditor;
	TSharedPtr<SWidgetSwitcher> TabsWidget;
	TMap<TWeakObjectPtr<UEdGraph>, TSharedPtr<SGraphEditor>> GraphToEditorMap;

public:
	SLATE_BEGIN_ARGS( SGraphView ) {}
	SLATE_END_ARGS()


	void Construct(
		const FArguments& InArgs, 
		TSharedPtr<class FEditor> InBlueprintEditor);

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	virtual FString GetReferencerName() const override
	{
		return TEXT("SGraphView");
	}

	virtual FReply OnKeyDown(const FGeometry& Geo, const FKeyEvent& Event) override;

private:
	void OnSelectionChanged(const TSet<class UObject*>& NewSelection);
	void OnGraphSelected(UEdStateGraph* Graph);
	void BindEvents(UStateMachineBlueprint* Blueprint);

	void AddGraphToEditor(UEdGraph* Graph);

	//virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
};