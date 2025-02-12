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

	UAssimpImporter();
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
	 * @param bDisableAutoSpaceChange
	 * @param OnProgress
	 * @param OnComplete
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Import Library", meta=( WorldContext="WorldContextObject"))
	static UPARAM(DisplayName = "Assimp Importer")UAssimpImporter* AssimpImportFiles(UObject* WorldContextObject,
		const TArray<FString>& InFileNames, const int Flags,
		const bool bDisableAutoSpaceChange,
		const FOnAssimpImportProgress& OnProgress,
		const FOnAssimpImportComplete& OnComplete);

	/**
	 * Canceling the current import
	 * It won't end right away
	 * @link Assimp::ProgressHandler::Update
	 */
	UFUNCTION(BlueprintCallable, Category = "Asset Import Library")
	void CancelImport();

protected:
	// Importing the files
	void AssimpImportFiles(const TArray<FString>& InFileNames);
	// Importing the file
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
	
	/** @brief Progress callback.
	 *  @param Percentage An estimate of the current loading progress,
	 *    in percent. Or -1.f if such an estimate is not available.
	 *
	 *  There are restriction on what you may do from within your
	 *  implementation of this method: no exceptions may be thrown and no
	 *  non-const #Importer methods may be called. It is
	 *  not generally possible to predict the number of callbacks
	 *  fired during a single import.
	 *
	 *  @return Return false to abort loading at the next possible
	 *   occasion (loaders and Assimp are generally allowed to perform
	 *   all needed cleanup tasks prior to returning control to the
	 *   caller). If the loading is aborted, #Importer::ReadFile()
	 *   returns always nullptr.
	 *   */
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
