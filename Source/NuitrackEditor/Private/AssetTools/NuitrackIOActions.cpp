// Fill out your copyright notice in the Description page of Project Settings.


#include "NuitrackIOActions.h"
#include "NuitrackIO.h"

FText FNuitrackIOActions::GetName() const
{
	return NSLOCTEXT("NuitrackIOActions", "AssetTypeActions_NuitrackIO", "Nuitrack IO");
}

FColor FNuitrackIOActions::GetTypeColor() const
{
	return FColor::White;
}

UClass* FNuitrackIOActions::GetSupportedClass() const
{
	return UNuitrackIO::StaticClass();
}

uint32 FNuitrackIOActions::GetCategories()
{
	return EAssetTypeCategories::Media;
}
