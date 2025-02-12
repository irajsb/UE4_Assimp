#include "Actions/AssimpImporter.h"

#include "AIScene.h"
#include "assimp/ai_assert.h"
#include "assimp/cimport.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/Exceptional.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "UE_AssimpLibrary/assimp/code/Common/ScenePrivate.h"

UAssimpImporter::UAssimpImporter(): Flags(0), DisableAutoSpaceChange(false), WorldPtr(nullptr),
                                    ProgressHandler(nullptr)
{
}

UAssimpImporter::~UAssimpImporter()
{
	UE_LOG(LogAssimp, Log, TEXT("Destroyed AssimpImporter %s"), *GetNameSafe(this))
	CancelImport();
}

UAssimpImporter* UAssimpImporter::AssimpImportFiles(UObject* WorldContextObject,
                                                    const TArray<FString>& InFileNames, const int Flags,
                                                    const bool bDisableAutoSpaceChange,
                                                    const FOnAssimpImportProgress& OnProgress,
                                                    const FOnAssimpImportComplete& OnComplete)
{
	UAssimpImporter* AssimpImporter = NewObject<UAssimpImporter>();
	AssimpImporter->AddToRoot();
	AssimpImporter->OnProgress = OnProgress;
	AssimpImporter->OnComplete = OnComplete;
	AssimpImporter->Flags = Flags;
	AssimpImporter->DisableAutoSpaceChange = bDisableAutoSpaceChange;
	AssimpImporter->WorldPtr = WorldContextObject;
	// AssimpImporter->ProgressHandler = new FAssimpProgressHandler();
	AssimpImporter->AssimpImportFiles(InFileNames);
	return AssimpImporter;
}

void UAssimpImporter::CancelImport()
{
	if (ProgressHandler && !ProgressHandler->IsTerminationRequested())
	{
		// @todo: It won't end right away
		ProgressHandler->RequestTermination();
		UE_LOG(LogAssimp, Log, TEXT("Cancelled import %s"), *GetNameSafe(this));
	}
}

void UAssimpImporter::AssimpImportFiles(const TArray<FString>& InFileNames)
{
	Async(EAsyncExecution::ThreadPool, [this, InFileNames]()
	{
		Assimp::DefaultLogger::set(new UEAssimpStream());

		for (int i = 0; i < InFileNames.Num(); i++)
		{
			if (ProgressHandler && ProgressHandler->IsTerminationRequested())
			{
				break;
			}
			UAIScene* AIScene = AssimpImportFile(InFileNames[i]);
			float Progress = static_cast<float>(i + 1) / InFileNames.Num();
			EAssimpImportResult Result = IsValid(AIScene)
				                             ? EAssimpImportResult::Success
				                             : EAssimpImportResult::InvalidAIScene;
			if (ProgressHandler && ProgressHandler->IsTerminationRequested())
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
				ProgressHandler->IsTerminationRequested()
					? EAssimpImportResult::Cancelled
					: EAssimpImportResult::Complete,
				this);
		});

		ConditionalBeginDestroy();
	});
}

UAIScene* UAssimpImporter::AssimpImportFile(const FString& InFileName)
{
	UAIScene* Object = nullptr;

	if (!DisableAutoSpaceChange)
	{
		Flags |= aiProcess_MakeLeftHanded | aiProcessPreset_TargetRealtime_Quality;
	}

	// The old implementation logic, but the import process cannot be terminated, so the read file logic is overridden based on this
	/*if (const aiScene* Scene = aiImportFile(TCHAR_TO_UTF8(*InFileName), static_cast<unsigned int>(Flags)))
	{
		Object = UAIScene::InternalConstructNewScene(Scene, DisableAutoSpaceChange);
		Object->FullFilePath = InFileName;
	}*/

	/**
	 * @brief The new implementation logic, but the import process can be terminated
	 * @link aiScene::aiImportFileExWithProperties
	 */
	const char* FilePtr = TCHAR_TO_UTF8(*InFileName);
	ai_assert(nullptr != FilePtr);

	ASSIMP_BEGIN_EXCEPTION_REGION()
		// create an Importer for this file
		Assimp::Importer* Importer = new Assimp::Importer();
		// set up a progress handler, use to terminate the import process
		ProgressHandler = new FAssimpProgressHandler();
		Importer->SetProgressHandler(ProgressHandler);

		// skip copy properties
		// skip set up a custom IO system if necessary

		// and have it read the file
		// if succeeded, store the importer in the scene and keep it alive
		if (const aiScene* Scene = Importer->ReadFile(FilePtr, static_cast<unsigned int>(Flags)))
		{
			Assimp::ScenePrivateData* ScenePrivateData =
				const_cast<Assimp::ScenePrivateData*>(Assimp::ScenePriv(Scene));
			ScenePrivateData->mOrigImporter = Importer;

			// create a AIScene for unreal
			Object = UAIScene::InternalConstructNewScene(Scene, DisableAutoSpaceChange);
			Object->FullFilePath = InFileName; // set full file path
		}
		else
		{
			// if failed, extract error code and destroy the import
			UE_LOG(LogAssimp, Error, TEXT("Failed to import file %s, ErrorString: %hs"),
			       *InFileName, Importer->GetErrorString())
			delete Importer;
		}

		// return imported data. If the import failed the pointer is nullptr anyway
	ASSIMP_END_EXCEPTION_REGION(UAIScene *);

	return Object;
}
