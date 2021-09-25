// Fill out your copyright notice in the Description page of Project Settings.


#include "NuitrackVisualizer.h"
#include "DrawDebugHelpers.h"
#include "Components/BillboardComponent.h"

DEFINE_LOG_CATEGORY(NuitrackVis);

// Sets default values
ANuitrackVisualizer::ANuitrackVisualizer() :
	ConfidenceThres(0.1),
	SkeletonWidth(2.0)
{
 	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	auto BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	BillboardComponent->SetHiddenInGame(true, true);
	BillboardComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ANuitrackVisualizer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANuitrackVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
#if WITH_EDITOR

	auto World = GetWorld();
	if (!World) return;

	FlushPersistentDebugLines(World);

	if (NuitrackIO)
	{
		if (NuitrackIO->IsOpen() && NuitrackIO->bUseSkeletonTracking)
		{
			int NumSkeletons = NuitrackIO->GetNumSkeleton();
			if (NumSkeletons == 0) return;

			const auto& Joints = NuitrackIO->GetSkeletons()[0].Joints;
			const auto& Confidences = NuitrackIO->GetSkeletons()[0].Confidences;
			const FTransform& Transform = GetActorTransform();

			for (const auto& Bone : UNuitrackIO::GetNuitrackBones())
			{
				uint8 StartID = static_cast<uint8>(Bone.ID);
				uint8 EndID = static_cast<uint8>(Bone.ParentID);

				if (Confidences[StartID] < ConfidenceThres || Confidences[EndID] < ConfidenceThres)
				{
					continue;
				}

				FVector Start(Joints[StartID].GetLocation());
				Start = Transform.TransformPosition(Start);

				FVector End(Joints[EndID].GetLocation());
				End = Transform.TransformPosition(End);

				DrawDebugLine(World, Start, End, SkeletonColor, true, -1, 0, SkeletonWidth);
				
			}
			
		}

	}
#endif

}

bool ANuitrackVisualizer::ShouldTickIfViewportsOnly() const
{
	if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Editor)
	{
		return true;
	}
	else
	{
		return false;
	}
}

