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

class FMenuBuilder;
class FScopedTransaction;
class SBox;
class SCanvas;
class SPaintSurface;
class SRuler;
class SZoomPan;
class UPanelWidget;
class UWidgetBlueprint;
class FHittestGrid;
struct FOnPaintHandlerParams;
struct FWidgetHitResult;
class UWidgetEditingProjectSettings;


class STATEMACHINEEDITOR_API SGraphView : public SCompoundWidget, public FGCObject
{

private:
	TSharedPtr<SGraphEditor> GraphEditor;
	//TSharedPtr<SVerticalBox> StateList;

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


	//virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
};