// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#define ToVector(In) FVector( In.y, In.x, In.z);
#define ToVectorCM(In) FVector( In.y*100, In.x*100, In.z*100);//convert Meters to cm
#define MatTranslation(In) FVector(In.b4*100,In.a4*100,In.c4*100)

UENUM(BlueprintType)
enum ETaskResult
{
	Success,
	Fail
};

class FUE_AssimpModule : public IModuleInterface
{
public:

	
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** Handle to the test dll we will load */

};
