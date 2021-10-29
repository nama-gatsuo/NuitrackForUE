// Fill out your copyright notice in the Description page of Project Settings.

#include "NuitrackIO.h"
#include "Runtime/RHI/Public/RHI.h"


DEFINE_LOG_CATEGORY(NuitrackIOLog);

using namespace tdv::nuitrack;

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
	AllocateUserColor();
}

UNuitrackIO::UNuitrackIO(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	DeviceIndex(-1),
	bOpen(false)
{
	Nuitrack::init();
	LoadDevices();
	AllocateUserColor();
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

		if (ColorTexture)
		{
			auto Sensor = ColorSensor::create();
			uint64_t Handler = Sensor->connectOnNewFrame(
				std::bind(&UNuitrackIO::OnColorUpdate, this, std::placeholders::_1)
			);

			Modules[ModuleType::COLOR_SENSOR] = Sensor;
			ModuleCallbackHandlers[ModuleType::COLOR_SENSOR] = Handler;
		}

		if (DepthTexture)
		{
			auto Sensor = DepthSensor::create();
			uint64_t Handler = Sensor->connectOnNewFrame(
				std::bind(&UNuitrackIO::OnDepthUpdate, this, std::placeholders::_1)
			);

			Modules[ModuleType::DEPTH_SENSOR] = Sensor;
			ModuleCallbackHandlers[ModuleType::DEPTH_SENSOR] = Handler;
		}

		if (UserTexture)
		{
			auto Tracker = UserTracker::create();
			uint64_t Handler = Tracker->connectOnUpdate(
				std::bind(&UNuitrackIO::OnUserUpdate, this, std::placeholders::_1)
			);

			Modules[ModuleType::USER_TRACKER] = Tracker;
			ModuleCallbackHandlers[ModuleType::USER_TRACKER] = Handler;
		}

		if (bUseSkeletonTracking)
		{
			auto Tracker = SkeletonTracker::create();
			Tracker->setNumActiveUsers(NumBodiesToTrack);

			uint64_t Handler = Tracker->connectOnUpdate(
				std::bind(&UNuitrackIO::OnSkeletonUpdate, this, std::placeholders::_1)
			);

			Modules[ModuleType::SKELETON_TRACKER] = Tracker;
			ModuleCallbackHandlers[ModuleType::SKELETON_TRACKER] = Handler;
		}

		if (bUseHandTracking)
		{
			auto Tracker = HandTracker::create();
			Modules[ModuleType::HAND_TRACKER] = Tracker;

			uint64_t Handler = Tracker->connectOnUpdate(
				std::bind(&UNuitrackIO::OnHandUpdate, this, std::placeholders::_1)
			);

			Modules[ModuleType::HAND_TRACKER] = Tracker;
			ModuleCallbackHandlers[ModuleType::HAND_TRACKER] = Handler;
		}

		if (bUseGestureRecognizer)
		{
			auto Tracker = GestureRecognizer::create();
			Modules[ModuleType::GESTURE_RECOGNIZER] = Tracker;

			uint64_t Handler = Tracker->connectOnUpdate(
				std::bind(&UNuitrackIO::OnGestureUpdate, this, std::placeholders::_1)
			);
			uint64_t Handler0 = Tracker->connectOnNewGestures(
				std::bind(&UNuitrackIO::OnNewGesture, this, std::placeholders::_1)
			);

			Modules[ModuleType::GESTURE_RECOGNIZER] = Tracker;
			ModuleCallbackHandlers[ModuleType::GESTURE_RECOGNIZER] = Handler;
		}

		Nuitrack::run();
	}
	catch (const Exception& exception)
	{
		FString Msg(ANSI_TO_TCHAR(exception.what()));
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
		if (Modules[ModuleType::SKELETON_TRACKER])
		{
			std::static_pointer_cast<SkeletonTracker>(Modules[ModuleType::SKELETON_TRACKER])->disconnectOnUpdate(
				ModuleCallbackHandlers[ModuleType::SKELETON_TRACKER]
			);
		}
		
		if (Modules[ModuleType::HAND_TRACKER])
		{
			std::static_pointer_cast<HandTracker>(Modules[ModuleType::HAND_TRACKER])->disconnectOnUpdate(
				ModuleCallbackHandlers[ModuleType::HAND_TRACKER]
			);
		}

		if (Modules[ModuleType::GESTURE_RECOGNIZER])
		{
			std::static_pointer_cast<GestureRecognizer>(Modules[ModuleType::GESTURE_RECOGNIZER])->disconnectOnUpdate(
				ModuleCallbackHandlers[ModuleType::GESTURE_RECOGNIZER]
			);
		}
		
		if (Modules[ModuleType::COLOR_SENSOR])
		{
			std::static_pointer_cast<ColorSensor>(Modules[ModuleType::COLOR_SENSOR])->disconnectOnNewFrame(
				ModuleCallbackHandlers[ModuleType::COLOR_SENSOR]
			);
		}

		if (Modules[ModuleType::DEPTH_SENSOR])
		{
			std::static_pointer_cast<DepthSensor>(Modules[ModuleType::DEPTH_SENSOR])->disconnectOnNewFrame(
				ModuleCallbackHandlers[ModuleType::DEPTH_SENSOR]
			);
		}

		if (Modules[ModuleType::USER_TRACKER])
		{
			std::static_pointer_cast<UserTracker>(Modules[ModuleType::USER_TRACKER])->disconnectOnUpdate(
				ModuleCallbackHandlers[ModuleType::USER_TRACKER]
			);
		}
		
		Modules.clear();
		ModuleCallbackHandlers.clear();

		Nuitrack::release();
	}
	catch (const Exception& exception)
	{
		FString Msg(ANSI_TO_TCHAR(exception.what()));
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

const TArray<FNuitrackUserGesture>& UNuitrackIO::GetGestures() const
{
	if (bOpen)
	{
		FScopeLock Lock(Thread->GetCriticalSection());
		return NuiGestures;
	}
	else
	{
		return NuiGestures;
	}
}

void UNuitrackIO::UpdateAsync()
{
	try
	{
		for (auto& Module : Modules)
		{
			Nuitrack::waitUpdate(Module.second);
		}
	}
	catch (tdv::nuitrack::LicenseNotAcquiredException& Exception)
	{
		FString Msg(ANSI_TO_TCHAR(Exception.what()));
		UE_LOG(NuitrackIOLog, Error, TEXT("License not acquired (%s)"), *Msg);
		Stop();
		return;
	}
	catch (const Exception& exception)
	{
		FString Msg(ANSI_TO_TCHAR(exception.what()));
		UE_LOG(NuitrackIOLog, Error, TEXT("Nuitrack update failed (%s)"), *Msg);
		return;
	}
}

void UNuitrackIO::OnSkeletonUpdate(SkeletonData::Ptr SkeletonsPtr)
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

void UNuitrackIO::OnHandUpdate(HandTrackerData::Ptr HandPtr)
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

			NuiHandPairs.Push(MoveTemp(HandPair));
		}
		
	}

}

void UNuitrackIO::OnColorUpdate(RGBFrame::Ptr ColorPtr)
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

void UNuitrackIO::OnDepthUpdate(tdv::nuitrack::DepthFrame::Ptr DepthPtr)
{
	if (DepthPtr)
	{
		int Width = DepthPtr->getCols(), Height = DepthPtr->getRows();
		if (Width == 0 || Height == 0) return;
		if (DepthTexture->GetSurfaceWidth() != Width || DepthTexture->GetSurfaceHeight() != Height)
		{
			DepthTexture->InitCustomFormat(Width, Height, EPixelFormat::PF_R8G8B8A8, false);
			DepthTexture->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
			DepthTexture->UpdateResource();
		}
		else
		{
			auto DepthSamples = DepthPtr->getData();
			TArray<uint8> SourceBuffer;
			SourceBuffer.Reserve(Width * Height * 4);
			for (int i = 0; i < Width * Height; i++)
			{

				SourceBuffer.Push(DepthSamples[i] & 0xFF);
				SourceBuffer.Push(uint8(DepthSamples[i] >> 8) & 0xFF);
				SourceBuffer.Push(DepthSamples[i] == 0 ? 0xFF : 0x00);
				SourceBuffer.Push(0xff);
			}

			FTextureResource* TextureResource = DepthTexture->Resource;
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

void UNuitrackIO::OnUserUpdate(tdv::nuitrack::UserFrame::Ptr UserFramePtr)
{
	if (UserFramePtr)
	{
		int Width = UserFramePtr->getCols(), Height = UserFramePtr->getRows();
		if (Width == 0 || Height == 0) return;
		if (UserTexture->GetSurfaceWidth() != Width || UserTexture->GetSurfaceHeight() != Height)
		{
			UserTexture->InitCustomFormat(Width, Height, EPixelFormat::PF_R8G8B8A8, false);
			UserTexture->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
			UserTexture->UpdateResource();
		}
		else
		{
			auto UserSamples = UserFramePtr->getData();
			TArray<uint8> SourceBuffer;
			SourceBuffer.Reserve(Width * Height * 4);
			for (int i = 0; i < Width * Height; i++)
			{
				int Index = UserSamples[i] % ColorsForUserTexture.Num();
				if (Index == 0)
				{
					for (int j = 0; j < 4; j++)
					{
						SourceBuffer.Push(0x00);
					}
				}
				else
				{
					const FColor& Col = ColorsForUserTexture[Index - 1];
					SourceBuffer.Push(Col.R);
					SourceBuffer.Push(Col.G);
					SourceBuffer.Push(Col.B);
					SourceBuffer.Push(0xFF);
				}
			}

			FTextureResource* TextureResource = UserTexture->Resource;
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

void UNuitrackIO::OnGestureUpdate(tdv::nuitrack::UserGesturesStateData::Ptr GesturePtr)
{
	if (!GesturePtr) return;

	FScopeLock Lock(Thread->GetCriticalSection());
	NuiGestures.Reset(GesturePtr->getUserGesturesStates().size());

	
	for (auto& State : GesturePtr->getUserGesturesStates())
	{
		FNuitrackUserGesture UserGesture;
		UserGesture.State = static_cast<ENuitrackUserStateType>(State.state);
		UserGesture.UserID = State.userId;
		for (auto& Gesture : State.gestures)
		{
			UserGesture.GestureProgress.Emplace(
				static_cast<ENuitrackGestureType>(Gesture.type), Gesture.progress
			);
		}

		NuiGestures.Push(MoveTemp(UserGesture));
	}

}

void UNuitrackIO::OnNewGesture(tdv::nuitrack::GestureData::Ptr GesturePtr)
{
	if (!GesturePtr) return;

	for (auto& Gesture : GesturePtr->getGestures())
	{
		OnUserGesture.Broadcast(Gesture.userId, static_cast<ENuitrackGestureType>(Gesture.type));
	}
}

FNuitrackSkeleton UNuitrackIO::BlendSkeleton(const FNuitrackSkeleton& A, const FNuitrackSkeleton& B, float alpha, bool bIgnoreConfidence)
{
	FNuitrackSkeleton Skeleton;

	if (A.Joints.Num() == B.Joints.Num())
	{
		Skeleton.Joints.Reset(A.Joints.Num());
		for (int i = 0; i < A.Joints.Num(); i++)
		{
			Skeleton.Joints.Push(
				FTransform(
					FQuat::Slerp(A.Joints[i].GetRotation(), B.Joints[i].GetRotation(), alpha),
					FMath::Lerp(A.Joints[i].GetLocation(), B.Joints[i].GetLocation(), alpha)
				));

			if (!bIgnoreConfidence)
			{
				Skeleton.Confidences.Push(FMath::Lerp(A.Confidences[i], B.Confidences[i], alpha));
			}
		}
		return Skeleton;
	}
	else
	{
		return A;
	}
}

FTransform UNuitrackIO::JointToTransform(const Joint& Joint)
{

	
	FVector Location(Joint.real.z, Joint.real.x, Joint.real.y);
	Location *= 0.1f;
	
	FVector X(Joint.orient.matrix[0], Joint.orient.matrix[3], Joint.orient.matrix[6]);
	FVector Y(Joint.orient.matrix[1], Joint.orient.matrix[4], Joint.orient.matrix[7]);
	FVector Z(Joint.orient.matrix[2], Joint.orient.matrix[5], Joint.orient.matrix[8]);
	
	FQuat RawRotation(FMatrix(FPlane(X, 0), FPlane(Y, 0), FPlane(Z, 0), FPlane(0,0,0, 1))); // Matrix to Quaternion
	FQuat Rotation;
	
	if (
		Joint.type == JOINT_LEFT_SHOULDER ||
		Joint.type == JOINT_LEFT_ELBOW ||
		Joint.type == JOINT_LEFT_WRIST
		)
	{
		Rotation = FQuat(RawRotation.X, -RawRotation.Z, RawRotation.Y, RawRotation.W);
	}
	else if (
		Joint.type == JOINT_RIGHT_SHOULDER ||
		Joint.type == JOINT_RIGHT_ELBOW ||
		Joint.type == JOINT_RIGHT_WRIST
	)
	{
		Rotation = FQuat(RawRotation.X, RawRotation.Z, -RawRotation.Y, RawRotation.W);
	}
	else if (
		Joint.type == JOINT_RIGHT_HIP ||
		Joint.type == JOINT_RIGHT_KNEE ||
		Joint.type == JOINT_RIGHT_ANKLE ||
		Joint.type == JOINT_RIGHT_FOOT
		)
	{
		Rotation = FQuat(RawRotation.Y, RawRotation.Z, -RawRotation.X, RawRotation.W);
	}
	else
	{
		Rotation = FQuat(RawRotation.Y, -RawRotation.Z, -RawRotation.X, RawRotation.W);
	}
	

	return FTransform(Rotation, Location);
}

void UNuitrackIO::AllocateUserColor()
{
	ColorsForUserTexture.Reset(6);
	ColorsForUserTexture.Push(FColor(255, 0, 0, 255));
	ColorsForUserTexture.Push(FColor(0, 255, 0, 255));
	ColorsForUserTexture.Push(FColor(0, 0, 255, 255));
	ColorsForUserTexture.Push(FColor(255, 255, 0, 255));
	ColorsForUserTexture.Push(FColor(255, 0, 255, 255));
	ColorsForUserTexture.Push(FColor(0, 255, 255, 255));
}
