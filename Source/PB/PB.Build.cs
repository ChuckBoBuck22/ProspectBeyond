// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PB : ModuleRules
{
	public PB(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", "Landscape", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", 
			"UMG", "Niagara", "PhysicsCore", "NavigationSystem", "AIModule","EnhancedInput", 
			"HTTP", "Json", "JsonUtilities", "Agones"});
	}
}
