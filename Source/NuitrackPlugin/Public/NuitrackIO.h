// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/TextureRenderTarget2D.h"

#include "nuitrack/Nuitrack.h"

#include "NuitrackEnum.h"
#include "NuitrackThread.h"

#include "NuitrackIO.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(NuitrackIOLog, Log, All);

USTRUCT(BlueprintType)
struct FNuitrackHand
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IsClick;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Pressure;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Position;
};

USTRUCT(BlueprintType)
struct FNuitrackHandPair
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FNuitrackHand Left;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FNuitrackHand Right;
	
};

USTRUCT(BlueprintType)
struct FNuitrackBone
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ENuitrackJoint ParentID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ENuitrackJoint ID;

};

USTRUCT(BlueprintType)
struct FNuitrackSkeleton
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 ID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FTransform> Joints;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<float> Confidences;
};

/**
 * 
 */
UCLASS(BlueprintType, hidecategories = (Object))
class NUITRACKPLUGIN_API UNuitrackIO : public UObject
{
	GENERATED_BODY()
public:

	using NuiSkeletonTracker = tdv::nuitrack::SkeletonTracker;
	using NuiSkeletonData = tdv::nuitrack::SkeletonData;
	using NuiHandTracker = tdv::nuitrack::HandTracker;
	using NuiHandData = tdv::nuitrack::HandTrackerData;
	using NuiJoint = tdv::nuitrack::Joint;
	using Nuitrack = tdv::nuitrack::Nuitrack;
	using NuiException = tdv::nuitrack::Exception;
	using NuiDevice = tdv::nuitrack::device::NuitrackDevice;
	using NuiColorSensor = tdv::nuitrack::ColorSensor;
	using NuiColorFrame = tdv::nuitrack::RGBFrame;

	UNuitrackIO();
	UNuitrackIO(const FObjectInitializer& ObjectInitializer);
	virtual ~UNuitrackIO();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Nuitrack | Config", meta = (UIMin="0", UIMax="6"))
	int32 NumBodiesToTrack;

	UPROPERTY(BlueprintReadWrite, Category = "Nuitrack | Config")
	int32 DeviceIndex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Nuitrack | Config")
	FString ActivationKey;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Nuitrack | Config")
	bool bUseSkeletonTracking;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Nuitrack | Config")
	bool bUseHandTracking;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Nuitrack | IO")
	UTextureRenderTarget2D* ColorTexture;

	UFUNCTION(BlueprintCallable, Category = "Nuitrack | IO")
	void LoadDevices();

	/*
	 * Before starting Nuitrack process, you should set ActivationKey in advance
	 * so that Nuitrack can varify.
	*/
	UFUNCTION(BlueprintCallable, Category = "Nuitrack | IO")
	bool Start();

	UFUNCTION(BlueprintCallable, Category = "Nuitrack | IO")
	bool Stop();

	UFUNCTION(BlueprintCallable, Category = "Nuitrack | IO")
	bool IsOpen() const;

	UFUNCTION(BlueprintCallable, Category = "Nuitrack | Skeleton")
	int32 GetNumSkeleton() const;

	UFUNCTION(BlueprintCallable, Category = "Nuitrack | Skeleton")
	const TArray<FNuitrackSkeleton>& GetSkeletons() const;

	UFUNCTION(BlueprintCallable, Category = "Nuitrack | Hands")
	int32 GetNumPairOfHands() const;

	UFUNCTION(BlueprintCallable, Category = "Nuitrack | Hands")
	const TArray<FNuitrackHandPair>& GetHandPairs() const;

	/*
	 * Nuitrack Bone represents pair of joints. So you can trace joints' hierarchy.
	*/ 
	UFUNCTION(BlueprintCallable, Category = "Nuitrack | Skeleton")
	static const TArray<FNuitrackBone>& GetNuitrackBones() { return Bones; }

	// Interface for thread
	void UpdateAsync();
	
	// Nuitrack callbacks
	void OnSkeletonUpdate(NuiSkeletonData::Ptr SkeletonsPtr);
	void OnHandUpdate(NuiHandData::Ptr SkeletonsPtr);
	void OnColorUpdate(NuiColorFrame::Ptr ColorPtr);

	// Interface for Editor Module
	TArray<TSharedPtr<FString>> DeviceList;

private:
	static FTransform JointToTransform(const NuiJoint& Joint);

	NuiSkeletonTracker::Ptr SkeletonTracker;
	uint64_t SkeletonUpdateHandler;
	NuiHandTracker::Ptr HandTracker;
	uint64_t HandUpdateHandler;
	NuiColorSensor::Ptr ColorSensor;
	uint64_t ColorUpdateHandler;

	FNuitrackThread* Thread;
	bool bOpen;

	int32 NumSkeletons;
	TArray<FNuitrackSkeleton> NuiSkeletons;

	int32 NumPairOfHands;
	TArray<FNuitrackHandPair> NuiHandPairs;

	TArray<NuiDevice::Ptr> NuiDevices;

	const static TArray<FNuitrackBone> Bones;

};
