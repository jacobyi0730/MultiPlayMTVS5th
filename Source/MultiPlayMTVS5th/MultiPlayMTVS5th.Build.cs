// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MultiPlayMTVS5th : ModuleRules
{
	public MultiPlayMTVS5th(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"MultiPlayMTVS5th",
			"MultiPlayMTVS5th/Variant_Platforming",
			"MultiPlayMTVS5th/Variant_Platforming/Animation",
			"MultiPlayMTVS5th/Variant_Combat",
			"MultiPlayMTVS5th/Variant_Combat/AI",
			"MultiPlayMTVS5th/Variant_Combat/Animation",
			"MultiPlayMTVS5th/Variant_Combat/Gameplay",
			"MultiPlayMTVS5th/Variant_Combat/Interfaces",
			"MultiPlayMTVS5th/Variant_Combat/UI",
			"MultiPlayMTVS5th/Variant_SideScrolling",
			"MultiPlayMTVS5th/Variant_SideScrolling/AI",
			"MultiPlayMTVS5th/Variant_SideScrolling/Gameplay",
			"MultiPlayMTVS5th/Variant_SideScrolling/Interfaces",
			"MultiPlayMTVS5th/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
