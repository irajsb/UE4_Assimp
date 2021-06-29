// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#define ToVector(In) FVector( In.x, In.y, In.z);
class FUE_AssimpModule : public IModuleInterface
{
public:

	
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** Handle to the test dll we will load */

};
