// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimNode_NuitrackPose.h"
#include "Animation/AnimInstanceProxy.h"


DEFINE_LOG_CATEGORY(NuitrackAnim);

FAnimNode_NuitrackPose::FAnimNode_NuitrackPose() :
	bUsePosition(false)
{

	BonesToModify.Reserve(24);
	for (int i = 1; i < 25; i++)
	{
		BonesToModify.Add(static_cast<ENuitrackJoint>(i), FBoneReference());
	}
}

void FAnimNode_NuitrackPose::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Update_AnyThread);

	GetEvaluateGraphExposedInputs().Execute(Context);

	//UE_LOG(NuitrackAnim, Warning, TEXT("Joints: "));

	USkeletalMeshComponent* SkelMesh = Context.AnimInstanceProxy->GetSkelMeshComponent();

	BoneTransforms.Reset(Skeleton.Joints.Num());

	for (int i = 0; i < Skeleton.Joints.Num(); i++)
	{
		ENuitrackJoint JointIndex = static_cast<ENuitrackJoint>(i);
		if (BonesToModify.Contains(JointIndex))
		{
			int32 BoneIndex = SkelMesh->GetBoneIndex(BonesToModify[JointIndex].BoneName);
			if (BoneIndex != INDEX_NONE)
			{
				FCompactPoseBoneIndex CompactBoneIndex(BoneIndex);
				BoneTransforms.Emplace(CompactBoneIndex, Skeleton.Joints[i]);
			}

		}
	}

	BoneTransforms.Sort([](const FBoneTransform& A, const FBoneTransform& B) {
		return A.BoneIndex < B.BoneIndex;
		});
}

void FAnimNode_NuitrackPose::EvaluateComponentSpace_AnyThread(FComponentSpacePoseContext& Output)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(EvaluateComponentSpace_AnyThread)

	Output.ResetToRefPose();

	for (const FBoneTransform& BoneTransform : BoneTransforms)
	{
		FTransform Transform = Output.Pose.GetComponentSpaceTransform(BoneTransform.BoneIndex);
		if (bUsePosition)
		{
			Transform.SetLocation(BoneTransform.Transform.GetLocation());
		}
		Transform.SetRotation(BoneTransform.Transform.GetRotation());
		Output.Pose.SetComponentSpaceTransform(BoneTransform.BoneIndex, Transform);
	}
}

void FAnimNode_NuitrackPose::Evaluate_AnyThread(FPoseContext& Output)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Evaluate_AnyThread)
	Output.ResetToRefPose();

	for (const FBoneTransform& BoneTransform : BoneTransforms)
	{
		if (bUsePosition)
		{
			Output.Pose[BoneTransform.BoneIndex].SetLocation(BoneTransform.Transform.GetLocation());
		}
		Output.Pose[BoneTransform.BoneIndex].SetRotation(BoneTransform.Transform.GetRotation());	
	}
	
}
