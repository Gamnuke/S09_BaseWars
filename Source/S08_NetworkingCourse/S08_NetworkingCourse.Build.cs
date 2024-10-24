// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class S08_NetworkingCourse : ModuleRules
{
	public S08_NetworkingCourse(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "OnlineSubsystem", "OnlineSubsystemSteam", "NavigationSystem", "SoundVisualizations", "SoundMod", "AdvancedSessions" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "SoundVisualizations"});
    }
}
