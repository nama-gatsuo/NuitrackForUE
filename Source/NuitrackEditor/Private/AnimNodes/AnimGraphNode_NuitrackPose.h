// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimGraphNode_Base.h"
#include "AnimNode_NuitrackPose.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "AnimNodeEditModes.h"

#include "AnimGraphNode_NuitrackPose.generated.h"

/**
 * 
 */
UCLASS(meta = (Kerwords="Azure Kinect"))
class UAnimGraphNode_NuitrackPose : public UAnimGraphNode_Base
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Settings")
	FAnimNode_NuitrackPose Node;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FString GetNodeCategory() const override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual void CreateOutputPins() override;

};
