#include "Ability/Animation/AnimMontageAbility.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimMontageAbility::Start_Inner_Implementation()
{
	if (this->Component)
	{
		if (auto AnimInst = this->Component->GetAnimInstance())
		{
			if (this->Montage)
			{
				Super::Start_Inner_Implementation();
				AnimInst->Montage_Play(Montage);

				// Unnecessary since finish is handled in AnimAbility.
				//AnimInst->OnAllMontageInstancesEnded.AddDynamic(this, &UAnimAbility::Finish);
			}
			else
			{
				UE_LOG(LogAbility, Error, TEXT("No animation selected for AnimMontageAbility"));
				this->Finish();
			}
		}
	}
}