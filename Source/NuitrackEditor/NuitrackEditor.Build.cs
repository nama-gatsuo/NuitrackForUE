// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class NuitrackEditor : ModuleRules
{
	public NuitrackEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		//OverridePackageType = PackageOverrideType.GameUncookedOnly;
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.AddRange(
			new string[]
			{
				"NuitrackEditor/Private",
				"NuitrackEditor/Private/AnimNodes",
				"NuitrackEditor/Private/AssetTools",
				"NuitrackEditor/Private/Customizations",
				"NuitrackEditor/Private/Factories",
			});

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"PropertyEditor",
				"InputCore",
				"NuitrackPlugin",
				"UnrealEd",
				"BlueprintGraph",
				"AnimGraph",
			});

	}
}
