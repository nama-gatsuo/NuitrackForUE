// Fill out your copyright notice in the Description page of Project Settings.

#include "NuitrackIOCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SComboBox.h"
#include "PropertyCustomizationHelpers.h"

#define LOCTEXT_NAMESPACE "NuitrackIOCustomization"

TSharedRef<IDetailCustomization> FNuitrackIOCustomization::MakeInstance()
{
	return MakeShareable(new FNuitrackIOCustomization());
}

void FNuitrackIOCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{

	// Retrieve target object
	TArray<TWeakObjectPtr<UObject>> Objects;
	DetailBuilder.GetObjectsBeingCustomized(Objects);
	if (Objects.Num() == 1)
	{
		NuitrackIO = Cast<UNuitrackIO>(Objects[0].Get());
	}
	else
	{
		return;
	}

	// Customize 'Config' category
	IDetailCategoryBuilder& ConfigCategory = DetailBuilder.EditCategory("Config");
	TAttribute<bool> CheckDeviceOpen = TAttribute<bool>::Create(TAttribute<bool>::FGetter::CreateLambda(
		[this]() {
			return !NuitrackIO->IsOpen();
		}));

	
	// Add Custom Row of Device selection
	CurrentOption = NuitrackIO->DeviceList[0];

	ConfigCategory.AddCustomRow(LOCTEXT("DeviceSelectionFilterString", "Device Selection"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("DeviceSelectionLabel", "Device Selection"))
	]
	.ValueContent()
	[
		SNew(SComboBox<TSharedPtr<FString>>)
		.IsEnabled(CheckDeviceOpen)
		.OptionsSource(&(NuitrackIO->DeviceList))
		.OnSelectionChanged_Raw(this, &FNuitrackIOCustomization::OnSelectionChanged)
		.OnGenerateWidget_Raw(this, &FNuitrackIOCustomization::MakeWidgetForOption)
		.InitiallySelectedItem(CurrentOption)
		[
			SNew(STextBlock)
			.Text(this, &FNuitrackIOCustomization::GetCurrentItemLabel)
		]
	];

	auto ActivationKey = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UNuitrackIO, ActivationKey));
	auto UseSkeletonTracking = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UNuitrackIO, bUseSkeletonTracking));
	auto UseHandTracking = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UNuitrackIO, bUseHandTracking));
	auto NumBodiesToTrack = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UNuitrackIO, NumBodiesToTrack));

	ConfigCategory.AddProperty(ActivationKey).IsEnabled(CheckDeviceOpen);
	ConfigCategory.AddProperty(UseSkeletonTracking).IsEnabled(CheckDeviceOpen);
	ConfigCategory.AddProperty(UseHandTracking).IsEnabled(CheckDeviceOpen);
	ConfigCategory.AddProperty(NumBodiesToTrack).IsEnabled(CheckDeviceOpen);
	
	// Customize 'IO' category
	IDetailCategoryBuilder& IOCategory = DetailBuilder.EditCategory("IO");

	// Add Custom Row of Execution buttons
	IOCategory.AddCustomRow(LOCTEXT("ButtonFilterString", "Function Buttons"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ExecutionLabel", "Execution"))
		]
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(FMargin(0.f, 2.f, 10.f, 2.f))
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("LoadButtonText", "LoadDevice"))
				.Visibility_Lambda([this]() {
					return NuitrackIO->IsOpen() ? EVisibility::Collapsed : EVisibility::Visible;
						})
				.OnClicked_Lambda([this]() {
					NuitrackIO->LoadDevices();
					return FReply::Handled();
				})
			]
		+ SHorizontalBox::Slot()
		.Padding(FMargin(0.f, 2.f, 10.0f, 2.f))
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("StartButtonText", "StartDevice"))
			.Visibility_Lambda([this]() {
				return NuitrackIO->IsOpen() ? EVisibility::Collapsed : EVisibility::Visible;
			})
			.OnClicked_Lambda([this]() {
				NuitrackIO->Start();
				return FReply::Handled();
			})
		]
		+ SHorizontalBox::Slot()
		.Padding(FMargin(0.f, 0, 10.f, 2.f))
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("StopButtonText", "StopDevice"))
				.Visibility_Lambda([this]() {
				return NuitrackIO->IsOpen() ? EVisibility::Visible : EVisibility::Collapsed;
			})
			.OnClicked_Lambda([this]() {
				NuitrackIO->Stop();
				return FReply::Handled();
			})
		]
	];

}

TSharedRef<SWidget> FNuitrackIOCustomization::MakeWidgetForOption(TSharedPtr<FString> InOption)
{
	return SNew(STextBlock).Text(FText::FromString(*InOption));
}

void FNuitrackIOCustomization::OnSelectionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type)
{
	CurrentOption = NewValue;
	// Also update UNuitrackIO's current index
	int32 IndexOfFound = NuitrackIO->DeviceList.Find(NewValue);
	if (IndexOfFound == INDEX_NONE)
	{
		NuitrackIO->DeviceIndex = -1;
	}
	else
	{
		NuitrackIO->DeviceIndex = IndexOfFound - 1;
	}
}

FText FNuitrackIOCustomization::GetCurrentItemLabel() const
{
	if (CurrentOption.IsValid())
	{
		return FText::FromString(*CurrentOption);
	}

	return LOCTEXT("InvalidComboEntryText", "No Device");
}



#undef LOCTEXT_NAMESPACE