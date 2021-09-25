// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "NuitrackIO.h"

/**
 * 
 */
class FNuitrackIOCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	
	// Callbacks for Device Selection List view
	TSharedRef<SWidget> MakeWidgetForOption(TSharedPtr<FString> InOption);
	void OnSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type);
	FText GetCurrentItemLabel() const;
	
private:

	TWeakObjectPtr<UNuitrackIO> NuitrackIO;
	TSharedPtr<FString> CurrentOption;
	
};
