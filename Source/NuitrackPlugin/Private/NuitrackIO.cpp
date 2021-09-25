// Fill out your copyright notice in the Description page of Project Settings.

#include "NuitrackIO.h"
#include "Runtime/RHI/Public/RHI.h"


DEFINE_LOG_CATEGORY(NuitrackIOLog);

const TArray<FNuitrackBone> UNuitrackIO::Bones({
//	{ ENuitrackJoint::JOINT_NONE,			ENuitrackJoint::JOINT_WAIST },
	{ ENuitrackJoint::JOINT_WAIST,			ENuitrackJoint::JOINT_TORSO },
	{ ENuitrackJoint::JOINT_TORSO,			ENuitrackJoint::JOINT_NECK },
	{ ENuitrackJoint::JOINT_NECK,			ENuitrackJoint::JOINT_HEAD },
	{ ENuitrackJoint::JOINT_LEFT_COLLAR,	ENuitrackJoint::JOINT_LEFT_SHOULDER },
	{ ENuitrackJoint::JOINT_LEFT_SHOULDER,	ENuitrackJoint::JOINT_LEFT_ELBOW },
	{ ENuitrackJoint::JOINT_LEFT_ELBOW,		ENuitrackJoint::JOINT_LEFT_WRIST },
	{ ENuitrackJoint::JOINT_LEFT_WRIST,		ENuitrackJoint::JOINT_LEFT_HAND },
//	{ ENuitrackJoint::JOINT_LEFT_HAND,		ENuitrackJoint::JOINT_LEFT_FINGERTIP },
	{ ENuitrackJoint::JOINT_RIGHT_COLLAR,	ENuitrackJoint::JOINT_RIGHT_SHOULDER },
	{ ENuitrackJoint::JOINT_RIGHT_SHOULDER,	ENuitrackJoint::JOINT_RIGHT_ELBOW },
	{ ENuitrackJoint::JOINT_RIGHT_ELBOW,	ENuitrackJoint::JOINT_RIGHT_WRIST },
	{ ENuitrackJoint::JOINT_RIGHT_WRIST,	ENuitrackJoint::JOINT_RIGHT_HAND },
//	{ ENuitrackJoint::JOINT_RIGHT_HAND,		ENuitrackJoint::JOINT_RIGHT_FINGERTIP },
	{ ENuitrackJoint::JOINT_WAIST,			ENuitrackJoint::JOINT_LEFT_HIP },
	{ ENuitrackJoint::JOINT_LEFT_HIP,		ENuitrackJoint::JOINT_LEFT_KNEE },
	{ ENuitrackJoint::JOINT_LEFT_KNEE,		ENuitrackJoint::JOINT_LEFT_ANKLE },
//	{ ENuitrackJoint::JOINT_LEFT_ANKLE,		ENuitrackJoint::JOINT_LEFT_FOOT },
	{ ENuitrackJoint::JOINT_WAIST,			ENuitrackJoint::JOINT_RIGHT_HIP },
	{ ENuitrackJoint::JOINT_RIGHT_HIP,		ENuitrackJoint::JOINT_RIGHT_KNEE },
	{ ENuitrackJoint::JOINT_RIGHT_KNEE,		ENuitrackJoint::JOINT_RIGHT_ANKLE },
//	{ ENuitrackJoint::JOINT_RIGHT_ANKLE,	ENuitrackJoint::JOINT_RIGHT_FOOT }
	});

UNuitrackIO::UNuitrackIO() :
	DeviceIndex(-1),
	bOpen(false)
{
	Nuitrack::init();
	LoadDevices();
}

UNuitrackIO::UNuitrackIO(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	DeviceIndex(-1),
	bOpen(false)
{
	Nuitrack::init();
	LoadDevices();
}

UNuitrackIO::~UNuitrackIO()
{
	Nuitrack::release();
}

void UNuitrackIO::LoadDevices()
{
	
	const auto& Devices = Nuitrack::getDeviceList();

	DeviceList.Reset(Devices.size() + 1);
	DeviceList.Add(MakeShared<FString>("No Device"));
	
	NuiDevices.Reset(Devices.size());

	for (auto Device : Devices)
	{
		std::string name(Device->getInfo(tdv::nuitrack::device::DeviceInfoType::DEVICE_NAME));
		std::string serial(Device->getInfo(tdv::nuitrack::device::DeviceInfoType::SERIAL_NUMBER));
		std::string provider(Device->getInfo(tdv::nuitrack::device::DeviceInfoType::PROVIDER_NAME));
		
		std::string item = name + " (" + provider + ") [ " + serial + " ]";
		DeviceList.Add(MakeShared<FString>(item.c_str()));
		NuiDevices.Add(Device);
	}

}

bool UNuitrackIO::Start()
{
	if (bOpen)
	{
		UE_LOG(NuitrackIOLog, Warning, TEXT("Nuitrack is already open."));
		return false;
	}

	if (DeviceIndex == -1)
	{
		UE_LOG(NuitrackIOLog, Warning, TEXT("No Device is selected."));
		return false;
	}


	try
	{
		NuiDevices[DeviceIndex]->activate(std::string(TCHAR_TO_UTF8(*ActivationKey)));
		Nuitrack::setDevice(NuiDevices[DeviceIndex]);

		if (bUseSkeletonTracking)
		{
			SkeletonTracker = NuiSkeletonTracker::create();
			SkeletonTracker->setNumActiveUsers(NumBodiesToTrack);
			SkeletonUpdateHandler = SkeletonTracker->connectOnUpdate(
				std::bind(&UNuitrackIO::OnSkeletonUpdate, this, std::placeholders::_1)
			);
		}

		if (bUseHandTracking)
		{
			HandTracker = NuiHandTracker::create();
			HandUpdateHandler = HandTracker->connectOnUpdate(
				std::bind(&UNuitrackIO::OnHandUpdate, this, std::placeholders::_1)
			);
		}

		if (ColorTexture)
		{
			ColorSensor = NuiColorSensor::create();
			ColorUpdateHandler = ColorSensor->connectOnNewFrame(
				std::bind(&UNuitrackIO::OnColorUpdate, this, std::placeholders::_1)
			);
		}

		Nuitrack::run();
	}
	catch (const NuiException& Exception)
	{
		FString Msg(ANSI_TO_TCHAR(Exception.what()));
		UE_LOG(NuitrackIOLog, Error, TEXT("Can not initialize Nuitrack (%s)"), *Msg);
		return false;
	}
	
	Thread = new FNuitrackThread(this);

	bOpen = true;

	return true;
}


bool UNuitrackIO::Stop()
{
	
	if (Thread)
	{
		Thread->EnsureCompletion();
		Thread = nullptr;
	}

	try
	{
		if (SkeletonTracker)
		{
			SkeletonTracker->disconnectOnUpdate(SkeletonUpdateHandler);
		}
		
		if (HandTracker)
		{
			HandTracker->disconnectOnUpdate(HandUpdateHandler);
		}
		
		if (ColorSensor)
		{
			ColorSensor->disconnectOnNewFrame(ColorUpdateHandler);
		}
		
		Nuitrack::release();
	}
	catch (const NuiException& Exception)
	{
		FString Msg(ANSI_TO_TCHAR(Exception.what()));
		UE_LOG(NuitrackIOLog, Error, TEXT("Can not release Nuitrack (%s)"), *Msg);
		return false;
	}

	bOpen = false;

	return true;
}

bool UNuitrackIO::IsOpen() const
{
	return bOpen;
}

int32 UNuitrackIO::GetNumSkeleton() const
{
	if (bOpen)
	{
		FScopeLock Lock(Thread->GetCriticalSection());
		return NumSkeletons;
	}
	else
	{
		return 0;
	}
	
}

const TArray<FNuitrackSkeleton>& UNuitrackIO::GetSkeletons() const
{

	if (bOpen)
	{
		FScopeLock Lock(Thread->GetCriticalSection());
		return NuiSkeletons;
	}
	else
	{
		return NuiSkeletons;
	}
	
}

int32 UNuitrackIO::GetNumPairOfHands() const
{
	if (bOpen)
	{
		FScopeLock Lock(Thread->GetCriticalSection());
		return NumPairOfHands;
	}
	else
	{
		return 0;
	}
}

const TArray<FNuitrackHandPair>& UNuitrackIO::GetHandPairs() const
{
	if (bOpen)
	{
		FScopeLock Lock(Thread->GetCriticalSection());
		return NuiHandPairs;
	}
	else
	{
		return NuiHandPairs;
	}
}

void UNuitrackIO::UpdateAsync()
{
	try
	{
		if (bUseSkeletonTracking && SkeletonTracker)
		{
			Nuitrack::waitUpdate(SkeletonTracker);
		}
		if (bUseHandTracking && HandTracker)
		{
			Nuitrack::waitUpdate(HandTracker);
		}
		if (ColorTexture && ColorSensor)
		{
			Nuitrack::waitUpdate(ColorSensor);
		}
	}
	catch (tdv::nuitrack::LicenseNotAcquiredException& Exception)
	{
		FString Msg(ANSI_TO_TCHAR(Exception.what()));
		UE_LOG(NuitrackIOLog, Error, TEXT("License not acquired (%s)"), *Msg);
		Stop();
		return;
	}
	catch (const NuiException& Exception)
	{
		FString Msg(ANSI_TO_TCHAR(Exception.what()));
		UE_LOG(NuitrackIOLog, Error, TEXT("Nuitrack update failed (%s)"), *Msg);
		return;
	}
}

void UNuitrackIO::OnSkeletonUpdate(NuiSkeletonData::Ptr SkeletonsPtr)
{
	if (SkeletonsPtr)
	{

		FScopeLock Lock(Thread->GetCriticalSection());

		NumSkeletons = SkeletonsPtr->getNumSkeletons();
		auto Skeletons = SkeletonsPtr->getSkeletons();

		NuiSkeletons.Reset(NumSkeletons);

		for (const auto& Skeleton : Skeletons)
		{
			FNuitrackSkeleton S;
			S.ID = Skeleton.id;
			S.Joints.Reset(Skeleton.joints.size());
			S.Confidences.Reset(Skeleton.joints.size());

			for (const auto& Joint : Skeleton.joints)
			{
				S.Joints.Push(JointToTransform(Joint));
				S.Confidences.Push(Joint.confidence);
			}
			
			NuiSkeletons.Push(S);
		}

	}
}

void UNuitrackIO::OnHandUpdate(NuiHandData::Ptr HandPtr)
{
	if (HandPtr)
	{
		FScopeLock Lock(Thread->GetCriticalSection());
		
		NumPairOfHands = HandPtr->getNumUsers();
		NuiHandPairs.Reset(NumPairOfHands);

		for (const auto& User : HandPtr->getUsersHands())
		{
			FNuitrackHandPair HandPair;

			HandPair.Left.IsClick = User.leftHand->click;
			HandPair.Left.Pressure = User.leftHand->pressure;
			HandPair.Left.Position = FVector(User.leftHand->zReal, User.leftHand->xReal, User.leftHand->yReal);

			HandPair.Right.IsClick = User.rightHand->click;
			HandPair.Right.Pressure = User.rightHand->pressure;
			HandPair.Right.Position = FVector(User.rightHand->zReal, User.rightHand->xReal, User.rightHand->yReal);

			NuiHandPairs.Push(HandPair);
		}
		
	}

}

void UNuitrackIO::OnColorUpdate(NuiColorFrame::Ptr ColorPtr)
{
	if (ColorPtr)
	{
		int Width = ColorPtr->getCols(), Height = ColorPtr->getRows();
		if (Width == 0 || Height == 0) return;
		if (ColorTexture->GetSurfaceWidth() != Width || ColorTexture->GetSurfaceHeight() != Height)
		{
			ColorTexture->InitCustomFormat(Width, Height, EPixelFormat::PF_R8G8B8A8, false);
			ColorTexture->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
			ColorTexture->UpdateResource();
		}
		else
		{
			auto Colors = ColorPtr->getData();
			TArray<uint8> SourceBuffer;
			SourceBuffer.Reserve(Width * Height * 4);
			for (int i = 0; i < Width * Height; i++)
			{
				SourceBuffer.Push(Colors[i].red);
				SourceBuffer.Push(Colors[i].green);
				SourceBuffer.Push(Colors[i].blue);
				SourceBuffer.Push(0xff);
			}

			FTextureResource* TextureResource = ColorTexture->Resource;
			auto Region = FUpdateTextureRegion2D(0, 0, 0, 0, Width, Height);

			ENQUEUE_RENDER_COMMAND(UpdateTextureData)(
				[TextureResource, Region, SourceBuffer](FRHICommandListImmediate& RHICmdList) {
					FTexture2DRHIRef Texture2D = TextureResource->TextureRHI ? TextureResource->TextureRHI->GetTexture2D() : nullptr;
					if (!Texture2D)
					{
						return;
					}
					RHIUpdateTexture2D(Texture2D, 0, Region, 4 * Region.Width, SourceBuffer.GetData());
				});
		}

	}
}

FTransform UNuitrackIO::JointToTransform(const NuiJoint& Joint)
{

	int JointType = Joint.type;
	FVector Location(Joint.real.z, Joint.real.x, Joint.real.y);
	Location *= 0.1f;
	Joint.orient;
	FVector X(Joint.orient.matrix[0], Joint.orient.matrix[3], Joint.orient.matrix[6]);
	FVector Y(Joint.orient.matrix[1], Joint.orient.matrix[4], Joint.orient.matrix[7]);
	FVector Z(Joint.orient.matrix[2], Joint.orient.matrix[5], Joint.orient.matrix[8]);
	
	FQuat RawRotation(FMatrix(FPlane(X, 0), FPlane(Y, 0), FPlane(Z, 0), FPlane(0,0,0, 1)));

	FQuat Rotation;

	/*
	bool IsLeftHand = JointType >= 6 && JointType <= 10;
	bool IsRightLeg = JointType >= 21 && JointType <= 24;

	if (IsRightLeg)
	{
		Rotation = FRotator(0, 180, 0).Quaternion() * FQuat(RawRotation.Y, -RawRotation.Z, RawRotation.X, RawRotation.W);
	}
	else if (IsLeftHand)
	{
		Rotation = FQuat(RawRotation.X, RawRotation.Y, RawRotation.Z, RawRotation.W);
	}
	else
	{
		Rotation = FQuat(RawRotation.Y, RawRotation.Z, RawRotation.X, RawRotation.W);
	}
	*/

	Rotation = FQuat(RawRotation.Y, RawRotation.Z, RawRotation.X, RawRotation.W);

	return FTransform(Rotation, Location);
}
