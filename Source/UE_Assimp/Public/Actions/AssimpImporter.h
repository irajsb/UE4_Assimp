#pragma once

#include "CoreMinimal.h"
#include "UE_Assimp.h"
#include "assimp/ProgressHandler.hpp"
#include "AssimpImporter.generated.h"

class FAssimpProgressHandler;

UENUM(BlueprintType)
enum class EAssimpImportResult : uint8
{
	Success,
	Cancelled,
	Complete,
	InvalidAIScene
};

/** Dynamic delegate to track import progress */
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnAssimpImportProgress, EAssimpImportResult, Result, UAIScene*, AISceneImported,
                                     float, ProgressRatio);

/** Dynamic delegate broadcast after the import is complete */
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAssimpImportComplete, EAssimpImportResult, Result, UAssimpImporter*, Importer);

class UAIScene;

/**
 * UAssimpImporter
 */
UCLASS(BlueprintType, Category = "Asset Import Library")
class UE_ASSIMP_API UAssimpImporter : public UObject
{
	GENERATED_BODY()

	virtual ~UAssimpImporter() override;

protected:
	FOnAssimpImportProgress OnProgress;
	FOnAssimpImportComplete OnComplete;

	int Flags;
	bool DisableAutoSpaceChange;

	UPROPERTY()
	UObject* WorldPtr;

public:
	/**
	 * Importing the files
	 * @param WorldContextObject
	 * @param InFileNames The files to import
	 * @param Flags The flags to use. You can use post process nodes and use | (bitwise Or node) between them to create any combination of flags. Also We recommend using preset flags. Flip UV flag is needed for correct unreal engine meshes
	 * @param DisableAutoSpaceChange
	 * @param OnProgress
	 * @param OnComplete
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Import Library", meta=( WorldContext="WorldContextObject"))
	static UPARAM(DisplayName = "Assimp Importer")UAssimpImporter* AssimpImportFiles(UObject* WorldContextObject,
		const TArray<FString>& InFileNames, const int Flags,
		bool DisableAutoSpaceChange,
		const FOnAssimpImportProgress& OnProgress,
		const FOnAssimpImportComplete& OnComplete);

	/**
	 * Canceling the current import
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Import Library")
	void CancelImport();

protected:
	void AssimpImportFiles(const TArray<FString>& InFileNames);
	UAIScene* AssimpImportFile(const FString& InFileName);

	// Progress handler, use to terminate the import process
	FAssimpProgressHandler* ProgressHandler;
};

class FAssimpProgressHandler : public Assimp::ProgressHandler
{
public:
	FAssimpProgressHandler() : bTerminationRequested(false)
	{
	}

	virtual bool Update(float Percentage) override
	{
		UE_LOG(LogAssimp, Log, TEXT("Percentage: %f, bTerminationRequested = %s"), Percentage * 100.0f,
		       bTerminationRequested ? TEXT("true") : TEXT("false"))
		if (bTerminationRequested)
		{
			return false; // Return false to cancel the import process
		}
		// Perform any other progress handling tasks here
		return true; // Continue the import process
	}

	void RequestTermination()
	{
		bTerminationRequested.store(true);
	}

	bool IsTerminationRequested() const
	{
		return bTerminationRequested.load();
	}

private:
	std::atomic<bool> bTerminationRequested;
};
