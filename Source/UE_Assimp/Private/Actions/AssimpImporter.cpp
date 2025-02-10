#include "Actions/AssimpImporter.h"

#include "AIScene.h"
#include "UE_Assimp.h"
#include "assimp/cimport.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/postprocess.h"

UAssimpImporter::~UAssimpImporter()
{
	CancelImport();
}

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
	AssimpImporter->Flags = Flags;
	AssimpImporter->DisableAutoSpaceChange = DisableAutoSpaceChange;
	AssimpImporter->WorldPtr = WorldContextObject;
	AssimpImporter->AssimpImportFiles(InFileNames);
	return AssimpImporter;
}

bool UAssimpImporter::CancelImport()
{
	bImportCancelled = true;
	UE_LOG(LogAssimp, Log, TEXT("Cancelled import %s"), *GetNameSafe(this));
	return true;
}

void UAssimpImporter::AssimpImportFiles(const TArray<FString>& InFileNames)
{
	Async(EAsyncExecution::ThreadPool, [this, InFileNames]()
	{
		Assimp::DefaultLogger::set(new UEAssimpStream());

		for (int i = 0; i < InFileNames.Num(); i++)
		{
			if (bImportCancelled)
			{
				break;
			}
			UAIScene* AIScene = AssimpImportFile(InFileNames[i]);
			float Progress = static_cast<float>(i + 1) / InFileNames.Num();
			EAssimpImportResult Result = IsValid(AIScene)
				                             ? EAssimpImportResult::Success
				                             : EAssimpImportResult::InvalidAIScene;
			if (bImportCancelled)
			{
				break;
			}
			AsyncTask(ENamedThreads::GameThread, [this, Result, AIScene, Progress]()
			{
				OnProgress.ExecuteIfBound(Result, AIScene, Progress);
			});
			UE_LOG(LogAssimp, Log, TEXT("Imported scene %d/%d : %s"), i + 1, InFileNames.Num(), *InFileNames[i])
		}

		Assimp::DefaultLogger::kill();

		RemoveFromRoot();
		AsyncTask(ENamedThreads::GameThread, [this]()
		{
			OnComplete.ExecuteIfBound(
				bImportCancelled ? EAssimpImportResult::Cancelled : EAssimpImportResult::Complete,
				this);
		});
	});
}

UAIScene* UAssimpImporter::AssimpImportFile(const FString& InFileName)
{
	if (!DisableAutoSpaceChange)
	{
		Flags |= aiProcess_MakeLeftHanded | aiProcessPreset_TargetRealtime_Quality;
	}

	if (const aiScene* Scene = aiImportFile(TCHAR_TO_UTF8(*InFileName), static_cast<unsigned int>(Flags)))
	{
		UAIScene* Object = UAIScene::InternalConstructNewScene(Scene, DisableAutoSpaceChange);
		Object->FullFilePath = InFileName;
		return Object;
	}

	UE_LOG(LogAssimp, Error, TEXT("Failed to import file %s"), *InFileName)
	return nullptr;
}
