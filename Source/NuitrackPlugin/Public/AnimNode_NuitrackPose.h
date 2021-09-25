// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BonePose.h"
#include "Animation/AnimNodeBase.h"
#include "NuitrackIO.h"

#include "AnimNode_NuitrackPose.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(NuitrackAnim, Log, All);

/**
 * Output Nuitrack pose in Component Space
 */
USTRUCT(BlueprintInternalUseOnly)
struct NUITRACKPLUGIN_API FAnimNode_NuitrackPose : public FAnimNode_Base
{
	GENERATED_BODY()
public:

	FAnimNode_NuitrackPose();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transform", meta = (PinShownByDefault))
	FNuitrackSkeleton Skeleton;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Transform")
	bool bUsePosition;

	// FAnimNode_Base interface
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void EvaluateComponentSpace_AnyThread(FComponentSpacePoseContext& Output) override;
	virtual void Evaluate_AnyThread(FPoseContext& Output) override;

	UPROPERTY(EditAnywhere, Category = "Bone Mapping")
	TMap<ENuitrackJoint, FBoneReference> BonesToModify;

private:
	TArray<FBoneTransform> BoneTransforms;
};
