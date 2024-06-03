#pragma once

#include "CoreMinimal.h"
#include "Editor/Blutility/Classes/ActorActionUtility.h"
#include "ActorManagement.generated.h"

UCLASS()
class CRABTOOLSUE5EDITOR_API UActorManagement : public UActorActionUtility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "ActorManagement")
	void ReplaceWithDoor() {}
};