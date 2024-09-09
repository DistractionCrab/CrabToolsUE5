#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityManager.generated.h"


UCLASS( ClassGroup=(General), meta=(BlueprintSpawnableComponent) )
class CRABTOOLSUE5_API UAbilityManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	virtual void TickComponent(
		float DeltaTime, 
		ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction)
		override;

protected:
	
	virtual void BeginPlay() override;

};
