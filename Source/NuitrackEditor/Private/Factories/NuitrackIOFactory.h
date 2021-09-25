// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Factories/Factory.h"
#include "UObject/ObjectMacros.h"

#include "NuitrackIOFactory.generated.h"

/**
 * Factory class of UNuitrackIO.
 * UNuitrackIO can be Asset Type due to this factory class.
 */
UCLASS(hidecategories=Object)
class UNuitrackIOFactory : public UFactory
{
	GENERATED_BODY()
public:
	UNuitrackIOFactory(const FObjectInitializer& ObjectInitializer);

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool ShouldShowInNewMenu() const override;
	virtual uint32 GetMenuCategories() const override;
};
