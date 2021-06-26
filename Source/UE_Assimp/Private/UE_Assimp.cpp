// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE_Assimp.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "assimp/Importer.hpp"     // C++ importer interface
#include "assimp/scene.h"          // Output data structure
#include "assimp/postprocess.h" // Post processing flags

#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FUE_AssimpModule"

void FUE_AssimpModule::StartupModule()
{


}

void FUE_AssimpModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUE_AssimpModule, UE_Assimp)
