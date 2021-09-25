

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

#include "NuitrackIO.h"
#include "NuitrackIOActions.h"
#include "NuitrackIOCustomization.h"


#define LOCTEXT_NAMESPACE "FNuitrackEditorModule"

class FNuitrackEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */

	virtual void StartupModule() override
	{
		NuitrackIOName = UNuitrackIO::StaticClass()->GetFName();
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

};

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNuitrackEditorModule, NuitrackEditor)