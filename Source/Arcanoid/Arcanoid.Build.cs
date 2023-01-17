// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Arcanoid : ModuleRules
{
	public Arcanoid(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Niagara" });

		PublicIncludePaths.AddRange(new string[] { "Arcanoid", "Arcanoid/Blocks", "Arcanoid/Bonus", "Arcanoid/Player", "Arcanoid/Game"});
	}
}
