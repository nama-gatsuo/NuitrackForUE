

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

#include "NuitrackIO.h"
#include "NuitrackIOActions.h"
#include "NuitrackIOCustomization.h"


#define LOCTEXT_NAMESPACE "FNuitrackEditorModule"
#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(StyleInstance->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)

class FNuitrackEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */

	virtual void StartupModule() override
	{
		NuitrackIOName = UNuitrackIO::StaticClass()->GetFName();

		const FVector2D Icon128x128(128.0f, 128.0f);
		const FVector2D Icon16x16(16.0f, 16.0f);

		StyleInstance = MakeUnique<FSlateStyleSet>("NuitrackEditorStyle");
		IPlugin* NuiTrackPlugin = IPluginManager::Get().FindPlugin("NuitrackPlugin").Get();
		if (NuiTrackPlugin)
		{
			StyleInstance->SetContentRoot(FPaths::Combine(NuiTrackPlugin->GetContentDir(), TEXT("Editor/Icons")));

			StyleInstance->Set("ClassThumbnail.NuitrackIO",	new IMAGE_BRUSH("Icon128", Icon128x128));
			StyleInstance->Set("ClassIcon.NuitrackIO",		new IMAGE_BRUSH("Icon16", Icon16x16));
			
			FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance.Get());
		}

		RegisterAssetTools();
		RegisterCustomizations();
	}
	
	virtual void ShutdownModule() override
	{
		UnregisterAssetTools();
		UnregisterCustomizations();
	}
	
protected:

	/** Registers asset tool actions. */
	void RegisterAssetTools()
	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		RegisterAssetTypeAction(AssetTools, MakeShareable(new FNuitrackIOActions()));
	}

	/** Unregisters asset tool actions. */
	void UnregisterAssetTools()
	{
		FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools");

		if (AssetToolsModule)
		{
			IAssetTools& AssetTools = AssetToolsModule->Get();

			for (auto Action : RegisteredAssetTypeActions)
			{
				AssetTools.UnregisterAssetTypeActions(Action);
			}
		}
	}

	/**
	 * Registers a single asset type action.
	 *
	 * @param AssetTools The asset tools object to register with.
	 * @param Action The asset type action to register.
	 */
	void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
	{
		AssetTools.RegisterAssetTypeActions(Action);
		RegisteredAssetTypeActions.Add(Action);
	}


	/** Register details view customizations. */
	void RegisterCustomizations()
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomClassLayout(NuitrackIOName, FOnGetDetailCustomizationInstance::CreateStatic(&FNuitrackIOCustomization::MakeInstance));
	}

	void UnregisterCustomizations()
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomClassLayout(NuitrackIOName);
	}

private:
	/** The collection of registered asset type actions. */
	TArray<TSharedRef<IAssetTypeActions>> RegisteredAssetTypeActions;

	FName NuitrackIOName;

	TUniquePtr<FSlateStyleSet> StyleInstance;

};

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNuitrackEditorModule, NuitrackEditor)