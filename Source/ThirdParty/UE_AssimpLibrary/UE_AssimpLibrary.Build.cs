// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class UE_AssimpLibrary : ModuleRules
{
	public UE_AssimpLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// Add the import library
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib", "Release", "assimp-vc142-mt.lib"));


			RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "bin","Release","assimp-vc142-mt.dll"));

			PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));
			// Delay-load the DLL, so we can load it from the right place first
		//	PublicDelayLoadDLLs.Add("ExampleLibrary.dll");

			// Ensure that the DLL is staged along with the executable
		//	RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/UE_AssimpLibrary/Win64/ExampleLibrary.dll");
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "Mac", "Release", "libExampleLibrary.dylib"));
            RuntimeDependencies.Add("$(PluginDir)/Source/ThirdParty/UE_AssimpLibrary/Mac/Release/libExampleLibrary.dylib");
        }
	}
}
