// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class FantasyRPG : ModuleRules
{
	public FantasyRPG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AIModule", "NavigationSystem", "GamePlayTasks", "UMG", "SlateCore", "Json" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

		// - 유니티 빌드 사용 안하도록함.
		// - 패키징할 때 해제하기
		// MinFilesUsingPrecompiledHeaderOverride = 1;
		// bFasterWithoutUnity = true;
	}
}
