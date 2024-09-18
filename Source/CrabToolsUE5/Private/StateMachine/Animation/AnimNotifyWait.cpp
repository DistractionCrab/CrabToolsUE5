#include "StateMachine/Animation/AnimNotifyWait.h"
#include "Components/SkeletalMeshComponent.h"
#include "StateMachine/Animation/Events.h"

UAnimNotifyWaitNode::UAnimNotifyWaitNode()
{
	this->AddEmittedEvent(Animation_Events::ANIM_NOTIFY_FINISH_WAIT);
	this->ActorClass = ACharacter::StaticClass();
}

void UAnimNotifyWaitNode::Initialize_Inner_Implementation()
{

	bool bFoundComp = false;

	for (auto& Components : this->GetOwner()->GetComponents())
	{
		if (auto SkComp = Cast<USkeletalMeshComponent>(Components))
		{
			if (Components->GetFName() == this->ComponentName) {
				this->Component = SkComp;
				bFoundComp = true;
			}
		}
	}

	if (!bFoundComp)
	{
		UE_LOG(LogStateMachine, Error, TEXT("Failed to find SkeletalMeshComponent %s from object %s"),
			*this->ComponentName.ToString(),
			*this->GetOwner()->GetName());
	}
}

void UAnimNotifyWaitNode::Enter_Inner_Implementation()
{
	if (this->Component)
	{
		if (auto AnimInst = this->Component->GetAnimInstance())
		{
			AnimInst->AddExternalNotifyHandler(this, "AnimNotify_SM_FinishWaitState");
		}
	}
}

void UAnimNotifyWaitNode::Exit_Inner_Implementation()
{
	if (this->Component)
	{
		if (auto AnimInst = this->Component->GetAnimInstance())
		{
			AnimInst->RemoveExternalNotifyHandler(this, "AnimNotify_SM_FinishWaitState");
		}
	}
}

TArray<FString> UAnimNotifyWaitNode::GetNotifyOptions() const
{
	TArray<FString> Names;

	/*
	this->Skeleton.LoadSynchronous();

	if (this->Skeleton == nullptr) { return Names; }	

	for (TObjectIterator<UAnimationAsset> Itr; Itr; ++Itr)
	{
		if (Skeleton->GetWorld() != (*Itr)->GetWorld()) { continue; }
		
		if (Skeleton == (*Itr)->GetSkeleton())
		{
			if (auto Seq = Cast<UAnimSequenceBase>(*Itr))
			{
				for (auto& Track : Seq->AnimNotifyTracks)
				{
					for (auto& Nots : Track.Notifies)
					{
						
						Names.Add(Nots->NotifyName.ToString());
					}
				}

			}
		}
	}*/
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

void UAnimNotifyWaitNode::AnimNotify_SM_FinishWaitState()
{
	this->EmitEvent(Animation_Events::ANIM_NOTIFY_FINISH_WAIT);
}