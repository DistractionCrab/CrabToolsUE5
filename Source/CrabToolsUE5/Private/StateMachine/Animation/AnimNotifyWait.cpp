#include "StateMachine/Animation/AnimNotifyWait.h"
#include "Engine/SkeletalMesh.h"

UAnimNotifyWaitNode::UAnimNotifyWaitNode()
{

}

void UAnimNotifyWaitNode::Initialize_Inner_Implementation()
{

}

TArray<FString> UAnimNotifyWaitNode::GetNotifyOptions() const
{
	TArray<FString> Names;

	if (Skeleton == nullptr) { return Names; }

	for (TObjectIterator<UAnimationAsset> Itr; Itr; ++Itr)
	{
		if (Skeleton->GetWorld() != (*Itr)->GetWorld()) { continue; }

		/*
		if (Skeleton == (*Itr)->GetSkeleton())
		{
			//AnimsArray.Add(*Itr);
		}
		*/
	}

	return Names;
}