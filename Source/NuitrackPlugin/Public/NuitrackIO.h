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
public:
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
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FNuitrackHand Left;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FNuitrackHand Right;
	
};

USTRUCT(BlueprintType)
struct FNuitrackBone
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ENuitrackJoint ParentID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ENuitrackJoint ID;

};

USTRUCT(BlueprintType)
struct FNuitrackSkeleton
{
	GENERATED_BODY()
public:
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Nuitrack | Config")
	bool bUseGestureRecognizer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Nuitrack | IO")
	UTextureRenderTarget2D* ColorTexture;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Nuitrack | IO")
	UTextureRenderTarget2D* DepthTexture;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Nuitrack | IO")
	UTextureRenderTarget2D* UserTexture;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Nuitrack | IO")
	TArray<FColor> ColorsForUserTexture;

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
	void OnSkeletonUpdate(tdv::nuitrack::SkeletonData::Ptr SkeletonsPtr);
	void OnHandUpdate(tdv::nuitrack::HandTrackerData::Ptr HandDataPtr);
	void OnColorUpdate(tdv::nuitrack::RGBFrame::Ptr ColorPtr);
	void OnDepthUpdate(tdv::nuitrack::DepthFrame::Ptr DepthPtr);
	void OnUserUpdate(tdv::nuitrack::UserFrame::Ptr UserPtr);
	void OnGestureUpdate(tdv::nuitrack::UserGesturesStateData::Ptr GesturePtr);

	// Interface for Editor Module
	TArray<TSharedPtr<FString>> DeviceList;

private:
	static FTransform JointToTransform(const tdv::nuitrack::Joint& Joint);
	
	void AllocateUserColor();

	enum class ModuleType : uint8
	{
		DEPTH_SENSOR = 0, COLOR_SENSOR, USER_TRACKER,
		SKELETON_TRACKER, HAND_TRACKER, GESTURE_RECOGNIZER
	};

	std::map<ModuleType, std::shared_ptr<tdv::nuitrack::HeaderOnlyAPI_Module>> Modules;
	std::map<ModuleType, uint64_t> ModuleCallbackHandlers;

	FNuitrackThread* Thread;
	bool bOpen;

	int32 NumSkeletons;
	TArray<FNuitrackSkeleton> NuiSkeletons;

	int32 NumPairOfHands;
	TArray<FNuitrackHandPair> NuiHandPairs;

	TArray<tdv::nuitrack::device::NuitrackDevice::Ptr> NuiDevices;

	const static TArray<FNuitrackBone> Bones;

};
