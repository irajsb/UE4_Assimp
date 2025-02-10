#include "UAssimpImporter.h"

/*bool UAssimpImporter::ImportModel(const FString& FilePath)
{
    FString AbsolutePath = FPaths::ConvertRelativePathToFull(FilePath);
    if (!FPaths::FileExists(AbsolutePath))
    {
        UE_LOG(LogTemp, Error, TEXT("File does not exist: %s"), *AbsolutePath);
        return false;
    }

    Assimp::Importer Importer;
    const aiScene* Scene = Importer.ReadFile(TCHAR_TO_UTF8(*AbsolutePath), aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
    {
        UE_LOG(LogTemp, Error, TEXT("Assimp error: %s"), UTF8_TO_TCHAR(Importer.GetErrorString()));
        return false;
    }

    // TODO: Process the imported scene data

    return true;
}*/

UAssimpImporter* UAssimpImporter::AssimpImportFiles(UObject* WorldContextObject,
                                                    const TArray<FString>& InFileNames, const int Flags,
                                                    bool DisableAutoSpaceChange,
                                                    const FOnAssimpImportProgress& OnProgress,
                                                    const FOnAssimpImportComplete& OnComplete)
{
	UAssimpImporter* AssimpImporter = NewObject<UAssimpImporter>();
	AssimpImporter->AddToRoot();
	AssimpImporter->OnProgress = OnProgress;
	AssimpImporter->OnComplete = OnComplete;
	AssimpImporter->AssimpImportFiles(WorldContextObject, InFileNames, Flags, DisableAutoSpaceChange);
	return AssimpImporter;
}

void UAssimpImporter::AssimpImportFiles(UObject* WorldContextObject, TArray<FString> InFileNames, int Flags,
                                        bool DisableAutoSpaceChange)
{
}
