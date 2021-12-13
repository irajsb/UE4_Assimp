// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class UE_AssimpLibrary : ModuleRules
{

	public string BinFolder(ReadOnlyTargetRules Target)
	{
		if(Target.Platform == UnrealTargetPlatform.Mac)
			return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../Binaries/Mac/"));
		else if(Target.Platform == UnrealTargetPlatform.IOS)
			return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../Binaries/IOS/"));
		if(Target.Platform == UnrealTargetPlatform.Win64)
			return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../Binaries/Win64/"));
		if(Target.Platform == UnrealTargetPlatform.Android)
			return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../Binaries/Android/"));
		return "";
	}
	
	public UE_AssimpLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// Add the import library
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory,"assimp" ,"lib", "Release", "assimp-vc142-mt.lib"));


			//RuntimeDependencies.Add(Path.Combine(ModuleDirectory,"assimp" , "bin","Release","assimp-vc142-mt.dll"));

			PublicIncludePaths.Add(Path.Combine(ModuleDirectory,"assimp" , "include"));
			// Delay-load the DLL, so we can load it from the right place first
			PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory,"assimp" , "bin","Release","assimp-vc142-mt.dll"));

			string BinaryFolder=BinFolder(Target);
			Directory.CreateDirectory(BinaryFolder);
			string  AssimpDll = Path.Combine(ModuleDirectory, "assimp", "bin", "Release", "assimp-vc142-mt.dll");
			string BinPath =Path.Combine(ModuleDirectory, BinaryFolder, "assimp-vc142-mt.dll");
			
		 CopyFile(AssimpDll,BinPath);
			  // Ensure that the DLL is staged along with the executable
		//	RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/UE_AssimpLibrary/Win64/ExampleLibrary.dll");
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
          
        }
	}
	public void CopyFile(string Source, string Dest)
	{
		System.Console.WriteLine("Copying {0} to {1}", Source, Dest);
		if (System.IO.File.Exists(Dest))
		{
			System.IO.File.SetAttributes(Dest, System.IO.File.GetAttributes(Dest) & ~System.IO.FileAttributes.ReadOnly);
		}
		try
		{
			//Make Folder
		
			System.IO.File.Copy(Source, Dest, true);
		}
		catch (System.Exception ex)
		{
			System.Console.WriteLine("Failed to copy file: {0}", ex.Message);
		}
	}

	
}
