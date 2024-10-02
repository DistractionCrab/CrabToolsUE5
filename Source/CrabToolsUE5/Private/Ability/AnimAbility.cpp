#include "Ability/AnimAbility.h"
#include "GameFramework/Character.h"

UAnimAbility::UAnimAbility()
{
	#if WITH_EDITORONLY_DATA
		this->ActorClass = ACharacter::StaticClass();
	#endif
}

void UAnimAbility::Initialize_Inner_Implementation()
{
	bool bFoundComp = false;

	for (auto& Components : this->GetOwner()->GetComponents())
	{
		if (auto SkComp = Cast<USkeletalMeshComponent>(Components))
		{
			if (Components->GetFName() == this->ComponentName)
			{
				this->Component = SkComp;
				bFoundComp = true;
				break;
			}
		}
	}
}

void UAnimAbility::Start_Inner_Implementation()
{
	if (this->Component)
	{
		if (auto AnimInst = this->Component->GetAnimInstance())
		{
			AnimInst->AddExternalNotifyHandler(
				this,
				GET_FUNCTION_NAME_CHECKED(UAnimAbility, AnimNotify_AbilityFinish));

			AnimInst->AddExternalNotifyHandler(
				this,
				GET_FUNCTION_NAME_CHECKED(UAnimAbility, AnimNotify_AbilityPerform));
		}
	}
}

void UAnimAbility::AnimNotify_AbilityFinish()
{
	this->Finish();
}

void UAnimAbility::AnimNotify_AbilityPerform()
{
	this->Perform();
}

#if WITH_EDITOR
TArray<FString> UAnimAbility::GetComponentOptions() const
{
	TArray<FString> Names;

	if (this->ActorClass)
	{
		for (auto& Components : Cast<AActor>(this->ActorClass->GetDefaultObject())->GetComponents())
		{
			if (auto SkComp = Cast<USkeletalMeshComponent>(Components))
			{
				Names.Add(SkComp->GetName());
			}
		}

		Names.Sort([&](const FString& A, const FString& B) { return A < B; });
	}

	return Names;
}
#endif //WITH_EDITOR