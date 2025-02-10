#pragma once

#include "CoreMinimal.h"
#include "AssimpImporter.generated.h"

UENUM(BlueprintType)
enum class EAssimpImportResult : uint8
{
	Success,
	Cancelled,
	InvalidAIScene
};

/** Dynamic delegate to track import progress */
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnAssimpImportProgress, EAssimpImportResult, Result, UAIScene*, AISceneImported, float, ProgressRatio);

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

protected:
	FOnAssimpImportProgress OnProgress;
	FOnAssimpImportComplete OnComplete;

	int Flags;
	bool DisableAutoSpaceChange;

	UPROPERTY()
	UObject* WorldPtr;

public:
	UFUNCTION(BlueprintCallable, Category = "Asset Import Library", meta=( WorldContext="WorldContextObject"))
	static UPARAM(DisplayName = "Assimp Importer")UAssimpImporter* AssimpImportFiles(UObject* WorldContextObject,
		const TArray<FString>& InFileNames, const int Flags,
		bool DisableAutoSpaceChange,
		const FOnAssimpImportProgress& OnProgress,
		const FOnAssimpImportComplete& OnComplete);

protected:
	void AssimpImportFiles(const TArray<FString>& InFileNames);
	UAIScene* AssimpImportFile(const FString& InFileName);
};
