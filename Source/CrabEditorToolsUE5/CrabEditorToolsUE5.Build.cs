// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CrabEditorToolsUE5 : ModuleRules
{
	public CrabEditorToolsUE5(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			});
				
		
		PrivateIncludePaths.AddRange(
			new string[] {

			});
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"CrabToolsUE5",
				"Core",
				"EnhancedInput",
				"UnrealEd",

				// ... add other public dependencies that you statically link with here ...
			});
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CrabToolsUE5",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"EnhancedInput",
				"AssetTools",
				"Slate",
				"InputCore",
				"SlateCore",
				"GraphEditor",
				"PropertyEditor",
				"EditorStyle",
				"Kismet",
				"KismetWidgets",
				"ApplicationCore",
				"ToolMenus",
				// ... add private dependencies that you statically link with here ...	
			});
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			});
	}
}
