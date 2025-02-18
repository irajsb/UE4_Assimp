// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "assimp/Logger.hpp"
#include "Modules/ModuleManager.h"
// rename ToVector to aiVector3DToVector, because the engine plugin "LidarPointCloud" already has a ToVector
#define aiVector3DToVector(In) FVector(In.x, In.y, In.z)
#define aiVector3DToVectorCM(In) FVector(In.x*100, In.y*100, In.z*100) //convert Meters to cm (DEPRECATED)
DECLARE_LOG_CATEGORY_EXTERN(LogAssimp, Log, All);



UENUM(BlueprintType)
enum class ETaskResult : uint8
{
	Success,
	Fail
};


// Example stream
class UEAssimpStream :
		public Assimp::Logger
{
public:
	// Constructor
	UEAssimpStream()
	{
		// empty
	}
        
	// Destructor
	virtual ~UEAssimpStream() override
	{
		// empty
	}
	// Write womethink using your own functionality
	virtual void OnError(const char* message) override
	{
		UE_LOG(LogAssimp,Error,TEXT("%s"),*FString(message));
	}
	virtual void OnDebug(const char* message) override
	{
		UE_LOG(LogAssimp,Log,TEXT("%s"),*FString(message));
	}
	virtual void OnInfo(const char* message) override
	{
		UE_LOG(LogAssimp,Log,TEXT("%s"),*FString(message));
	}
	virtual void OnWarn(const char* message) override
	{
		UE_LOG(LogAssimp,Warning,TEXT("%s"),*FString(message));
	}

	virtual void OnVerboseDebug(const char* message) override
	{
		UE_LOG(LogAssimp,Warning,TEXT("%s"),*FString(message));
	}
	virtual bool attachStream(Assimp::LogStream* pStream, unsigned severity) override
	{
		return false;
	}
	virtual bool detachStream(Assimp::LogStream* pStream, unsigned severity) override
	{
		return false;
	}
};
// ----------------------------------------------------------------------------------
/** Standard return type for some library functions.
* Rarely used, and if, mostly in the C API.
*/
UENUM(Blueprintable,BlueprintType)
 enum EAssimpReturn {
	/** Indicates that a function was successful */
	ReturnSuccess = 0,

    /** Indicates that a function failed */
  ReturnFail = -1,

    /** Indicates that not enough memory was available
    * to perform the requested operation
    */
    ReturnOutOfMemory = -3,



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
