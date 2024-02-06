// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PartyProject : ModuleRules
{
	public PartyProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "UMG"});
	}
}
