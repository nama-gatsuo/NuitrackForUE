// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "NuitrackEnum.generated.h"

/**
 * Blueprintable enum defined based on tdv::nuitrack::Joint from Skeleton.h
 */
UENUM(BlueprintType, Category = "Nuitrack|Enums")
enum class ENuitrackJoint : uint8
{
	JOINT_NONE = 0			UMETA(DisplayName = "Reserved Joint (unused)"),
	JOINT_HEAD				UMETA(DisplayName = "Head"),	///< Head
	JOINT_NECK				UMETA(DisplayName = "Neck"),	///< Neck
	JOINT_TORSO				UMETA(DisplayName = "Torso"),	///< Torso
	JOINT_WAIST				UMETA(DisplayName = "Waist"),	///< Waist
	JOINT_LEFT_COLLAR		UMETA(DisplayName = "Left Collar"), ///< Left collar
	JOINT_LEFT_SHOULDER		UMETA(DisplayName = "Left Shoulder"), ///< Left shoulder
	JOINT_LEFT_ELBOW		UMETA(DisplayName = "Left Elbow"), ///< Left elbow
	JOINT_LEFT_WRIST		UMETA(DisplayName = "Left Wrist"), ///< Left wrist
	JOINT_LEFT_HAND			UMETA(DisplayName = "Left Hand"), ///< Left hand
	JOINT_LEFT_FINGERTIP	UMETA(DisplayName = "Left Fingertip"), ///< Left fingertip (<b>not used in the current version</b>).
	JOINT_RIGHT_COLLAR		UMETA(DisplayName = "Right Collar"), ///< Right collar
	JOINT_RIGHT_SHOULDER	UMETA(DisplayName = "Right Shoulder"), ///< Right shoulder
	JOINT_RIGHT_ELBOW		UMETA(DisplayName = "Right Elbow"), ///< Right elbow
	JOINT_RIGHT_WRIST		UMETA(DisplayName = "Right Wrist"), ///< Right wrist
	JOINT_RIGHT_HAND		UMETA(DisplayName = "Right Hand"), ///< Right hand
	JOINT_RIGHT_FINGERTIP	UMETA(DisplayName = "Right Fingertip"), ///< Right fingertip (<b>not used in the current version</b>).
	JOINT_LEFT_HIP			UMETA(DisplayName = "Left Hip"), ///< Left hip
	JOINT_LEFT_KNEE			UMETA(DisplayName = "Left Knee"), ///< Left knee
	JOINT_LEFT_ANKLE		UMETA(DisplayName = "Left Ankle"), ///< Left ankle
	JOINT_LEFT_FOOT			UMETA(DisplayName = "Left Foot"), ///< Left foot (<b>not used in the current version</b>).
	JOINT_RIGHT_HIP			UMETA(DisplayName = "Right Hip"), ///< Right hip
	JOINT_RIGHT_KNEE		UMETA(DisplayName = "Right Knee"), ///< Right knee
	JOINT_RIGHT_ANKLE		UMETA(DisplayName = "Right Ankle"), ///< Right ankle
	JOINT_RIGHT_FOOT		UMETA(DisplayName = "Right Foot") ///< Right foot (<b>not used in the current version</b>).
};

UENUM(BlueprintType, Category = "Nuitrack|Enums")
enum class ENuitrackHand : uint8
{
	LEFT = 0,
	RIGHT
};