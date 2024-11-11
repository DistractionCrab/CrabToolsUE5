#include "Ability/Animation/AnimMontageAbility.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

UAnimMontageAbility::UAnimMontageAbility()
{
	#if WITH_EDITORONLY_DATA
		this->ActorClass = ACharacter::StaticClass();
	#endif

		this->ComponentName = "CharacterMesh0"; // Default character mesh name.
}

void UAnimMontageAbility::Initialize_Inner_Implementation()
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

	if (!bFoundComp)
	{
		UE_LOG(LogAbility, Error, TEXT("Failed to find SkeletalMeshComponent %s from object %s"),
			*this->ComponentName.ToString(),
			*this->GetOwner()->GetName());
	}
}

void UAnimMontageAbility::Start_Inner_Implementation()
{
	if (this->Component)
	{
		if (auto AnimInst = this->Component->GetAnimInstance())
		{
			AnimInst->AddExternalNotifyHandler(
				this,
				GET_FUNCTION_NAME_CHECKED(UAnimMontageAbility, AnimNotify_AbilityPerform));

			if (this->Montage)
			{
				AnimInst->Montage_Play(Montage);
				AnimInst->OnAllMontageInstancesEnded.AddDynamic(this, &UAnimMontageAbility::MontageEnded);
			}
			else
			{
				UE_LOG(LogAbility, Error, TEXT("No animation selected for AnimMontageAbility"));
				this->Finish();
			}
		}
	}
}

void UAnimMontageAbility::Finish_Inner_Implementation()
{
	if (this->Component)
	{
		if (auto AnimInst = this->Component->GetAnimInstance())
		{
			AnimInst->RemoveExternalNotifyHandler(
				this,
				GET_FUNCTION_NAME_CHECKED(UAnimMontageAbility, AnimNotify_AbilityPerform));
		}
	}
}

#if WITH_EDITOR
TArray<FString> UAnimMontageAbility::GetComponentOptions() const
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

void UAnimMontageAbility::AnimNotify_AbilityPerform()
{
	this->Perform();
}
