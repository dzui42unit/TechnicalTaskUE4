// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SphereHorde : ModuleRules
{
	public SphereHorde(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem" });
	}
}
