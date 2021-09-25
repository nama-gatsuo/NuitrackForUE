// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimGraphNode_NuitrackPose.h"
#include "AnimationGraphSchema.h"

#define LOCTEXT_NAMESPACE "NuitrackPose"

FText UAnimGraphNode_NuitrackPose::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("NuitrackPose", "Nuitrack Pose");
}

FString UAnimGraphNode_NuitrackPose::GetNodeCategory() const
{

	return FString("Nuitrack");
}

FText UAnimGraphNode_NuitrackPose::GetTooltipText() const
{
	return LOCTEXT(
		"AnimGraphNode_NuitrackPose_Tooltip",
		"Process Nuitrack skeleton input into pose"
	);
}

FLinearColor UAnimGraphNode_NuitrackPose::GetNodeTitleColor() const
{
	return FLinearColor(0.f, 0.f, 0.f);
}

void UAnimGraphNode_NuitrackPose::CreateOutputPins()
{
	CreatePin(EGPD_Output, UAnimationGraphSchema::PC_Struct, FPoseLink::StaticStruct(), TEXT("Pose"));
	//CreatePin(EGPD_Output, UAnimationGraphSchema::PC_Struct, FComponentSpacePoseLink::StaticStruct(), TEXT("ComponentPose"));
}

#undef LOCTEXT_NAMESPACE