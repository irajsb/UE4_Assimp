 // Fill out your copyright notice in the Description page of Project Settings.


#include "AssimpFunctionLibrary.h"
#include "AIScene.h"
#include "assimp/cimport.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"



#if PLATFORM_WINDOWS
#include "Windows/MinWindows.h"
#include "Misc/FeedbackContextMarkup.h"

#if ENGINE_MINOR_VERSION >26
#include "Microsoft/COMPointer.h"
#else
#include "Windows/COMPointer.h"
#endif

#include "Misc/Paths.h"
#include "Misc/Guid.h"
#include "HAL/FileManager.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include <commdlg.h>
#include <shellapi.h>
#include <shlobj.h>
#include <Winver.h>
#include <LM.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif


 void UAssimpFunctionLibrary::OpenFileDialogue(FString DialogTitle,FString DefaultPath,FString DefaultFile, const FString& FileTypes, uint8 Flags, TArray<FString>& OutFilenames,bool MultiSelect,bool &Success)
 {
 	void* ParentWindowHandle=nullptr;
 	int OutFilterIndex;
 Success=	FileDialogShared(false, ParentWindowHandle, DialogTitle, DefaultPath, DefaultFile, FileTypes, Flags, OutFilenames, OutFilterIndex,MultiSelect );

 }


bool UAssimpFunctionLibrary::FileDialogShared(bool bSave, const void* ParentWindowHandle, const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags, TArray<FString>& OutFilenames, int32& OutFilterIndex,bool MultiSelect)
{
	//FScopedSystemModalMode SystemModalScope;

	bool bSuccess = false;

	TComPtr<IFileDialog> FileDialog;
	if (SUCCEEDED(::CoCreateInstance(bSave ? CLSID_FileSaveDialog : CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, bSave ? IID_IFileSaveDialog : IID_IFileOpenDialog, IID_PPV_ARGS_Helper(&FileDialog))))
	{
		if (bSave)
		{
			// Set the default "filename"
			if (!DefaultFile.IsEmpty())
			{
				FileDialog->SetFileName(*FPaths::GetCleanFilename(DefaultFile));
			}
		}
		else
		{
			// Set this up as a multi-select picker
			if (MultiSelect)
			{
				DWORD dwFlags = 0;
				FileDialog->GetOptions(&dwFlags);
				FileDialog->SetOptions(dwFlags | FOS_ALLOWMULTISELECT);
			}
		}

		// Set up common settings
		FileDialog->SetTitle(*DialogTitle);
		if (!DefaultPath.IsEmpty())
		{
			// SHCreateItemFromParsingName requires the given path be absolute and use \ rather than / as our normalized paths do
			FString DefaultWindowsPath = FPaths::ConvertRelativePathToFull(DefaultPath);
			DefaultWindowsPath.ReplaceInline(TEXT("/"), TEXT("\\"), ESearchCase::CaseSensitive);

			TComPtr<IShellItem> DefaultPathItem;
			if (SUCCEEDED(::SHCreateItemFromParsingName(*DefaultWindowsPath, nullptr, IID_PPV_ARGS(&DefaultPathItem))))
			{
				FileDialog->SetFolder(DefaultPathItem);
			}
		}

		// Set-up the file type filters
		TArray<FString> UnformattedExtensions;
		TArray<COMDLG_FILTERSPEC> FileDialogFilters;
		{
			// Split the given filter string (formatted as "Pair1String1|Pair1String2|Pair2String1|Pair2String2") into the Windows specific filter struct
			FileTypes.ParseIntoArray(UnformattedExtensions, TEXT("|"), true);

			if (UnformattedExtensions.Num() % 2 == 0)
			{
				FileDialogFilters.Reserve(UnformattedExtensions.Num() / 2);
				for (int32 ExtensionIndex = 0; ExtensionIndex < UnformattedExtensions.Num();)
				{
					COMDLG_FILTERSPEC& NewFilterSpec = FileDialogFilters[FileDialogFilters.AddDefaulted()];
					NewFilterSpec.pszName = *UnformattedExtensions[ExtensionIndex++];
					NewFilterSpec.pszSpec = *UnformattedExtensions[ExtensionIndex++];
				}
			}
		}
		FileDialog->SetFileTypes(FileDialogFilters.Num(), FileDialogFilters.GetData());

		// Show the picker
		if (SUCCEEDED(FileDialog->Show((HWND)ParentWindowHandle)))
		{
			OutFilterIndex = 0;
			if (SUCCEEDED(FileDialog->GetFileTypeIndex((UINT*)&OutFilterIndex)))
			{
				OutFilterIndex -= 1; // GetFileTypeIndex returns a 1-based index
			}

			auto AddOutFilename = [&OutFilenames](const FString& InFilename)
			{
				FString& OutFilename = OutFilenames[OutFilenames.Add(InFilename)];
				OutFilename = IFileManager::Get().ConvertToRelativePath(*OutFilename);
				FPaths::NormalizeFilename(OutFilename);
			};

			if (bSave)
			{
				TComPtr<IShellItem> Result;
				if (SUCCEEDED(FileDialog->GetResult(&Result)))
				{
					PWSTR pFilePath = nullptr;
					if (SUCCEEDED(Result->GetDisplayName(SIGDN_FILESYSPATH, &pFilePath)))
					{
						bSuccess = true;

						// Apply the selected extension if the given filename doesn't already have one
						FString SaveFilePath = pFilePath;
						if (FileDialogFilters.IsValidIndex(OutFilterIndex))
						{
							// May have multiple semi-colon separated extensions in the pattern
							const FString ExtensionPattern = FileDialogFilters[OutFilterIndex].pszSpec;
							TArray<FString> SaveExtensions;
							ExtensionPattern.ParseIntoArray(SaveExtensions, TEXT(";"));

							// Build a "clean" version of the selected extension (without the wildcard)
							FString CleanExtension = SaveExtensions[0];
							if (CleanExtension == TEXT("*.*"))
							{
								CleanExtension.Reset();
							}
							else
							{
								int32 WildCardIndex = INDEX_NONE;
								if (CleanExtension.FindChar(TEXT('*'), WildCardIndex))
								{
									CleanExtension.RightChopInline(WildCardIndex + 1, false);
								}
							}

							// We need to split these before testing the extension to avoid anything within the path being treated as a file extension
							FString SaveFileName = FPaths::GetCleanFilename(SaveFilePath);
							SaveFilePath = FPaths::GetPath(SaveFilePath);

							// Apply the extension if the file name doesn't already have one
							if (FPaths::GetExtension(SaveFileName).IsEmpty() && !CleanExtension.IsEmpty())
							{
								SaveFileName = FPaths::SetExtension(SaveFileName, CleanExtension);
							}

							SaveFilePath /= SaveFileName;
						}
						AddOutFilename(SaveFilePath);

						::CoTaskMemFree(pFilePath);
					}
				}
			}
			else
			{
				IFileOpenDialog* FileOpenDialog = static_cast<IFileOpenDialog*>(FileDialog.Get());

				TComPtr<IShellItemArray> Results;
				if (SUCCEEDED(FileOpenDialog->GetResults(&Results)))
				{
					DWORD NumResults = 0;
					Results->GetCount(&NumResults);
					for (DWORD ResultIndex = 0; ResultIndex < NumResults; ++ResultIndex)
					{
						TComPtr<IShellItem> Result;
						if (SUCCEEDED(Results->GetItemAt(ResultIndex, &Result)))
						{
							PWSTR pFilePath = nullptr;
							if (SUCCEEDED(Result->GetDisplayName(SIGDN_FILESYSPATH, &pFilePath)))
							{
								bSuccess = true;
								AddOutFilename(pFilePath);
								::CoTaskMemFree(pFilePath);
							}
						}
					}
				}
			}
		}
	}

	return bSuccess;
}

 void UAssimpFunctionLibrary::ImportScenesAsync(TArray<FString> InFilenames,UObject* ParentObject,FOnProgressUpdated OnProgressUpdated,FOnImportSceneComplete OnImportSceneComplete)
 {

//I'm a noob in realms of async if you find a better way to keep data do a pull request
 	static float StartTime=0.0f;
 	static  int NumOfThreads=0;
 	static int TotalThreads=0;
 	NumOfThreads=TotalThreads=InFilenames.Num();
 	static TArray<UAIScene*> AIScenes;
 	AIScenes.Empty();
	StartTime=ParentObject->GetWorld()->GetTimeSeconds();
 
if(NumOfThreads==0)
{
	return;
}
 		

 		
 			
 			
 			for( FString FileName:InFilenames)
 			{

 				
 		
 				AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask,[ParentObject,FileName,OnProgressUpdated,OnImportSceneComplete]()
				{ 


 			
 				
 						const struct aiScene* scene = aiImportFile( TCHAR_TO_UTF8( *FileName),
						aiProcess_CalcTangentSpace       |
							aiProcess_Triangulate            |
							aiProcess_JoinIdenticalVertices  |
								aiProcess_SortByPType|
								aiProcess_FlipUVs);
 				if( !scene) {
				UE_LOG(LogTemp,Error,TEXT("Error importing scene in assimpfunction library async"))
 	
 					}else
 					{
 						
					
						AsyncTask(ENamedThreads::GameThread,[ParentObject,scene,OnProgressUpdated,OnImportSceneComplete]()
						{
							
							UAIScene* Object=	UAIScene::InternalConstructNewScene(ParentObject,scene);
								
							NumOfThreads=NumOfThreads-1;
							AIScenes.Add(Object);
							OnProgressUpdated.Execute(1-static_cast<float>(NumOfThreads)/TotalThreads,Object);
						
							if(NumOfThreads==0)
							{
							const	float EndTime=ParentObject->GetWorld()->GetTimeSeconds();
							const float TotalTime=EndTime-StartTime;
								OnImportSceneComplete.Execute(AIScenes,TotalTime);
								UE_LOG(LogTemp,Log,TEXT("start %f end %f  total %f"),StartTime,EndTime,TotalTime);
								
							}
						});
 					}
 				
 					
			});

 				
 			}
 		


 
 	
 	
 	

 	
 }

 void UAssimpFunctionLibrary::ImportScenes(TArray<FString> InFilenames, UObject* ParentObject,
	 TArray<UAIScene*>& Scenes)
 {
 	

 	for( FString FileName:InFilenames)
 	{
 		const struct aiScene* scene = aiImportFile( TCHAR_TO_UTF8( *FileName),
	aiProcess_CalcTangentSpace       |
	aiProcess_Triangulate            |
	aiProcess_JoinIdenticalVertices  |
	aiProcess_SortByPType|
	aiProcess_FlipUVs);
 		
 		
 				
 		if( !scene) {
 			UE_LOG(LogTemp,Error,TEXT("Error importing scene in assimpfunction library "))
 	
 				}
 			else
 			{
 				UAIScene* Object=	UAIScene::InternalConstructNewScene(ParentObject,scene);
 				
 			
 				Scenes.Add(Object);
 			
		}
 	}
 }

 FTransform UAssimpFunctionLibrary::aiMatToTransform(aiMatrix4x4 NodeTransform)
 {

 	//TODO Maybe We need to swap x and y  
 	//convert aiMatrix to UE  Matrix then to transform
 	FMatrix Matrix;
    
     FVector Ax1=FVector(NodeTransform.a1,NodeTransform.b1,NodeTransform.c1);
     FVector Ax2=FVector(NodeTransform.a2,NodeTransform.b2,NodeTransform.c2);
     FVector Ax3=FVector(NodeTransform.a3,NodeTransform.b3,NodeTransform.c3);
     FVector Ax4=FVector(NodeTransform.a4,NodeTransform.b4,NodeTransform.c4);
    Matrix.SetAxes(&Ax1,&Ax2,&Ax3,&Ax4);
 	return FTransform(Matrix);
 }

 FString UAssimpFunctionLibrary::GetBoneName(FAIBone Bone)
 {
 	return UTF8_TO_TCHAR(	Bone.Bone->mName.C_Str());
 }

 int UAssimpFunctionLibrary::GetNumOfWeights(FAIBone Bone)
 {
 	return Bone.Bone->mNumWeights;
 }

 FTransform UAssimpFunctionLibrary::GetBoneTransform(FAIBone Bone)
 {
 	return  UAssimpFunctionLibrary::aiMatToTransform(Bone.Bone->mOffsetMatrix);
 }

 void UAssimpFunctionLibrary::GetBoneWeights(FAIBone Bone, TArray<FAIVertexWeight>& Weights)
 {
 	for (unsigned int  i = 0; i < Bone.Bone->mNumWeights; i++)
 	{

 	 FAIVertexWeight Weight=	FAIVertexWeight(Bone.Bone->mWeights[i]);
 		Weights.Add(Weight);
 	}
 }

