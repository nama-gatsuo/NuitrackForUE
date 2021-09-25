// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "NuitrackIO.h"

#include "NuitrackVisualizer.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(NuitrackVis, Log, All);

UCLASS()
class NUITRACKPLUGIN_API ANuitrackVisualizer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANuitrackVisualizer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool ShouldTickIfViewportsOnly() const override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UNuitrackIO* NuitrackIO;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (UIMin="0.0", UIMax="1.0"))
	float ConfidenceThres;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FColor SkeletonColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SkeletonWidth;
};
