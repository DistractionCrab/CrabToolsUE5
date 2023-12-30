#pragma once

#include "WidgetCompare.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UWidgetOrderInterface : public UInterface
{
	GENERATED_BODY()
};

class IWidgetOrderInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Widget")
	bool Compare(UWidget* Widget);
	bool Compare_Implementation(UWidget* Widget) { return true; }
};
