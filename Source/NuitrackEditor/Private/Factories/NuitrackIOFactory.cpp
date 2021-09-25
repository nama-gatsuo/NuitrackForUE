// Fill out your copyright notice in the Description page of Project Settings.


#include "NuitrackIOFactory.h"
#include "NuitrackIO.h"
#include "AssetTypeCategories.h"

UNuitrackIOFactory::UNuitrackIOFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UNuitrackIO::StaticClass();
}

UObject* UNuitrackIOFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UNuitrackIO>(InParent, InClass, InName, Flags);
}

bool UNuitrackIOFactory::ShouldShowInNewMenu() const
{
	return true;
}

uint32 UNuitrackIOFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Misc;
}
