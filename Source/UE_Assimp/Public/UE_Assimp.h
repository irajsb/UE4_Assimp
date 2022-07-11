// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#define ToVector(In) FVector( In.y, In.x, In.z);
#define ToVectorCM(In) FVector( In.y*100, In.x*100, In.z*100);//convert Meters to cm
#define MatTranslation(In) FVector(In.b4*100,In.a4*100,In.c4*100)
DECLARE_LOG_CATEGORY_EXTERN(LogAssimp, Log, All);
UENUM(BlueprintType)
enum ETaskResult
{
	Success,
	Fail
};

// ----------------------------------------------------------------------------------
/** Standard return type for some library functions.
* Rarely used, and if, mostly in the C API.
*/
UENUM(Blueprintable,BlueprintType)
 enum EAssimpReturn {
	/** Indicates that a function was successful */
	ReturnSuccess = 0x0,

    /** Indicates that a function failed */
  ReturnFail = -0x1,

    /** Indicates that not enough memory was available
    * to perform the requested operation
    */
    ReturnOutOfMemory = -0x3,



    /// @endcond
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
