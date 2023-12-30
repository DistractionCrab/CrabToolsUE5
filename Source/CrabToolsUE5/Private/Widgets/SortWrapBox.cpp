#include "Widgets/SortWrapBox.h"
#include "Widgets/WidgetCompare.h"
#include "Components/WrapBoxSlot.h"
#include "Widgets/Layout/SWrapBox.h"

void USortWrapBox::SortChildren() {
	this->Slots.Sort([&](const UPanelSlot& A, const UPanelSlot& B) {
		if (A.Content && A.Content->GetClass()->ImplementsInterface(UWidgetOrderInterface::StaticClass())) {
			return IWidgetOrderInterface::Execute_Compare(A.Content, B.Content);
		}
		else {
			return false;
		}
	});

	if (MyWrapBox.IsValid())
	{
		for (auto& s : this->Slots) {
			TSharedPtr<SWidget> Widget = s->Content->GetCachedWidget();
			if (Widget.IsValid())
			{
				MyWrapBox->RemoveSlot(Widget.ToSharedRef());
			}
		}
		for (auto& s : this->Slots) {
			CastChecked<UWrapBoxSlot>(s)->BuildSlot(MyWrapBox.ToSharedRef());
		}
	}

	InvalidateLayoutAndVolatility();
}