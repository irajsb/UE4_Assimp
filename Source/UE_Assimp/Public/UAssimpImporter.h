#pragma once

#include "CoreMinimal.h"
#include "UAssimpImporter.generated.h"

/** Dynamic delegate to track import progress */
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAssimpImportProgress, UAIScene*, AISceneImported, float, ProgressRatio);

/** Dynamic delegate broadcast after the import is complete */
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAssimpImportComplete, ETaskResult, Result, UAssimpImporter*, Importer);

class UAIScene;

/**
 * UAssimpImporter
 */
UCLASS(BlueprintType, Category = "Asset Import Library")
class UE_ASSIMP_API UAssimpImporter : public UObject
{
	GENERATED_BODY()

protected:
	FOnAssimpImportProgress OnProgress;
	FOnAssimpImportComplete OnComplete;

public:
	UFUNCTION(BlueprintCallable, Category = "Asset Import Library", meta=( WorldContext="WorldContextObject"))
	static UAssimpImporter* AssimpImportFiles(UObject* WorldContextObject,
	                                          const TArray<FString>& InFileNames, const int Flags,
	                                          bool DisableAutoSpaceChange,
	                                          const FOnAssimpImportProgress& OnProgress,
	                                          const FOnAssimpImportComplete& OnComplete);

protected:
	void AssimpImportFiles(UObject* WorldContextObject,
	                       TArray<FString> InFileNames, int Flags, bool DisableAutoSpaceChange);
};
