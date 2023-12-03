#include "Components/LockOnCamera/LockOnAnchor.h"


ULockOnAnchor::ULockOnAnchor(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer) {
	this->LockOnDisplay = this->CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnDisplay"));
	this->LockOnDisplay->SetupAttachment(this);

	this->LockOnDisplay->SetWidgetSpace(EWidgetSpace::Screen);
}


void ULockOnAnchor::ActivateWidget_Implementation() {
	auto Widget = this->LockOnDisplay->GetWidget();

	if (Widget && Widget->GetClass()->ImplementsInterface(ULockOnInterface::StaticClass())) {
		ILockOnInterface::Execute_ActivateWidget(Widget);
	}
}

void ULockOnAnchor::DeactivateWidget_Implementation() {
	auto Widget = this->LockOnDisplay->GetWidget();

	if (Widget && Widget->GetClass()->ImplementsInterface(ULockOnInterface::StaticClass())) {
		ILockOnInterface::Execute_DeactivateWidget(Widget);
	}
}