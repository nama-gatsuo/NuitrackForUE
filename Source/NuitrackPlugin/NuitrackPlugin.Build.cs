using UnrealBuildTool;

public class NuitrackPlugin : ModuleRules
{
	public NuitrackPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"NuitrackModule"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"RenderCore",
			"RHI",
			"AnimGraphRuntime"
		});
    }
}
