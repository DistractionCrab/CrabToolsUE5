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
				"DeveloperSettings",
				// ... add other public dependencies that you statically link with here ...
			});
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"CrabToolsUE5",
				"KismetCompiler",
				"EditorFramework",
				"UnrealEd",
                "GraphEditor",
				"SlateCore",
                "Kismet",
                "KismetWidgets",
                "PropertyEditor",
				"ToolMenus",
				"AssetTools",
				"EditorSubsystem",
				"BlueprintGraph",
			});
	}
}
