// Fill out your copyright notice in the Description page of Project Settings.

#include "AssimpFunctionLibrary.h"

#include "AIBone.h"
#include "AIScene.h"
#include "UE_Assimp.h"
#include "assimp/cimport.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/scene.h"

#if PLATFORM_WINDOWS
#include <Runtime\Core\Public\HAL\FileManager.h>
#include <Runtime\Core\Public\Misc\Paths.h>
#include <Runtime\Core\Public\Windows\COMPointer.h>
#endif

#define MAX_FILETYPES_STR 4096
#define MAX_FILENAME_STR 65536

void UAssimpFunctionLibrary::OpenFileDialogue(FString DialogTitle, FString DefaultPath, FString DefaultFile,
											  const FString &FileTypes, uint8 Flags, TArray<FString> &OutFilenames,
											  bool MultiSelect, bool &Success)
{
	const void *ParentWindowHandle = nullptr;
	int OutFilterIndex;
	Success = FileDialogShared(false, ParentWindowHandle, DialogTitle, DefaultPath, DefaultFile, FileTypes, Flags,
							   OutFilenames, OutFilterIndex);
}

bool UAssimpFunctionLibrary::FileDialogShared(bool bSave, const void *ParentWindowHandle, const FString &DialogTitle,
											  const FString &DefaultPath, const FString &DefaultFile,
											  const FString &FileTypes, uint32 Flags, TArray<FString> &OutFilenames,
											  int32 &OutFilterIndex)
{
#pragma region Windows
	// FScopedSystemModalMode SystemModalScope;
#if PLATFORM_WINDOWS
	WCHAR Filename[MAX_FILENAME_STR];
	FCString::Strcpy(Filename, MAX_FILENAME_STR, *(DefaultFile.Replace(TEXT("/"), TEXT("\\"))));

	// Convert the forward slashes in the path name to backslashes, otherwise it'll be ignored as invalid and use whatever is cached in the registry
	WCHAR Pathname[MAX_FILENAME_STR];
	FCString::Strcpy(Pathname, MAX_FILENAME_STR,
					 *(FPaths::ConvertRelativePathToFull(DefaultPath).Replace(TEXT("/"), TEXT("\\"))));

	// Convert the "|" delimited list of filetypes to NULL delimited then add a second NULL character to indicate the end of the list
	WCHAR FileTypeStr[MAX_FILETYPES_STR];
	const WCHAR *FileTypesPtr = nullptr;
	const int32 FileTypesLen = FileTypes.Len();

	// Nicely formatted file types for lookup later and suitable to append to filenames without extensions
	TArray<FString> CleanExtensionList;

	// The strings must be in pairs for windows.
	// It is formatted as follows: Pair1String1|Pair1String2|Pair2String1|Pair2String2
	// where the second string in the pair is the extension.  To get the clean extensions we only care about the second string in the pair
	TArray<FString> UnformattedExtensions;
	FileTypes.ParseIntoArray(UnformattedExtensions, TEXT("|"), true);
	for (int32 ExtensionIndex = 1; ExtensionIndex < UnformattedExtensions.Num(); ExtensionIndex += 2)
	{
		const FString &Extension = UnformattedExtensions[ExtensionIndex];
		// Assume the user typed in an extension or doesnt want one when using the *.* extension. We can't determine what extension they wan't in that case
		if (Extension != TEXT("*.*"))
		{
			// Add to the clean extension list, first removing the * wildcard from the extension
			const int32 WildCardIndex = Extension.Find(TEXT("*"));
			CleanExtensionList.Add(WildCardIndex != INDEX_NONE ? Extension.RightChop(WildCardIndex + 1) : Extension);
		}
	}

	if (FileTypesLen > 0 && FileTypesLen - 1 < MAX_FILETYPES_STR)
	{
		FileTypesPtr = FileTypeStr;
		FCString::Strcpy(FileTypeStr, MAX_FILETYPES_STR, *FileTypes);

		TCHAR *Pos = FileTypeStr;
		while (Pos[0] != 0)
		{
			if (Pos[0] == '|')
			{
				Pos[0] = 0;
			}

			Pos++;
		}

		// Add two trailing NULL characters to indicate the end of the list
		FileTypeStr[FileTypesLen] = 0;
		FileTypeStr[FileTypesLen + 1] = 0;
	}

	OPENFILENAME ofn;
	FMemory::Memzero(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = (HWND)ParentWindowHandle;
	ofn.lpstrFilter = FileTypesPtr;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = Filename;
	ofn.nMaxFile = MAX_FILENAME_STR;
	ofn.lpstrInitialDir = Pathname;
	ofn.lpstrTitle = *DialogTitle;
	if (FileTypesLen > 0)
	{
		ofn.lpstrDefExt = &FileTypeStr[0];
	}

	ofn.Flags = OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER;

	if (bSave)
	{
		ofn.Flags |= OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_NOVALIDATE;
	}
	else
	{
		ofn.Flags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	}

	if (Flags & 0x01)
	{
		ofn.Flags |= OFN_ALLOWMULTISELECT;
	}

	bool bSuccess;
	if (bSave)
	{
		bSuccess = !!::GetSaveFileName(&ofn);
	}
	else
	{
		bSuccess = !!::GetOpenFileName(&ofn);
	}

	if (bSuccess)
	{
		// GetOpenFileName/GetSaveFileName changes the CWD on success. Change it back immediately.
		// FPlatformProcess::SetCurrentWorkingDirectoryToBaseDir();

		if (Flags & 0x01)
		{
			// When selecting multiple files, the returned string is a NULL delimited list
			// where the first element is the directory and all remaining elements are filenames.
			// There is an extra NULL character to indicate the end of the list.
			FString DirectoryOrSingleFileName = FString(Filename);
			TCHAR *Pos = Filename + DirectoryOrSingleFileName.Len() + 1;

			if (Pos[0] == 0)
			{
				// One item selected. There was an extra trailing NULL character.
				OutFilenames.Add(DirectoryOrSingleFileName);
			}
			else
			{
				// Multiple items selected. Keep adding filenames until two NULL characters.
				FString SelectedFile;
				do
				{
					SelectedFile = FString(Pos);
					new (OutFilenames) FString(DirectoryOrSingleFileName / SelectedFile);
					Pos += SelectedFile.Len() + 1;
				} while (Pos[0] != 0);
			}
		}
		else
		{
			new (OutFilenames) FString(Filename);
		}

		// The index of the filter in OPENFILENAME starts at 1.
		OutFilterIndex = ofn.nFilterIndex - 1;

		// Get the extension to add to the filename (if one doesnt already exist)
		FString Extension = CleanExtensionList.IsValidIndex(OutFilterIndex)
								? CleanExtensionList[OutFilterIndex]
								: TEXT("");

		// Make sure all filenames gathered have their paths normalized and proper extensions added
		for (auto OutFilenameIt = OutFilenames.CreateIterator(); OutFilenameIt; ++OutFilenameIt)
		{
			FString &OutFilename = *OutFilenameIt;

			OutFilename = IFileManager::Get().ConvertToRelativePath(*OutFilename);

			if (FPaths::GetExtension(OutFilename).IsEmpty() && !Extension.IsEmpty())
			{
				// filename does not have an extension. Add an extension based on the filter that the user chose in the dialog
				OutFilename += Extension;
			}

			FPaths::NormalizeFilename(OutFilename);
		}
	}
	else
	{
		uint32 Error = ::CommDlgExtendedError();
		if (Error != ERROR_SUCCESS)
		{
			// UE_LOG(LogDesktopPlatform, Warning, TEXT("Error reading results of file dialog. Error: 0x%04X"), Error);
		}
	}

	return bSuccess;
#endif
#pragma endregion

#pragma region LINUX
#if PLATFORM_LINUX
	return false;
#endif
#pragma endregion
	return false;
}

void UAssimpFunctionLibrary::ImportScenes(TArray<FString> InFilenames, UObject *ParentObject,
										  TArray<UAIScene *> &Scenes, int Flags)
{
	Assimp::DefaultLogger::set(new UEAssimpStream());

	for (FString FileName : InFilenames)
	{
		const struct aiScene *scene = aiImportFile(TCHAR_TO_UTF8(*FileName), Flags);

		if (!scene)
		{
			UE_LOG(LogAssimp, Error, TEXT("Error importing scene in assimpfunction library "))
		}
		else
		{
			UAIScene *Object = UAIScene::InternalConstructNewScene(ParentObject, scene);

			Scenes.Add(Object);
		}
	}
}

FTransform UAssimpFunctionLibrary::aiMatToTransform(aiMatrix4x4 NodeTransform)
{
	aiVector3t<float> Scale, Position;
	aiQuaterniont<float> Rotation;
	NodeTransform.Decompose(Scale, Rotation, Position);
	FRotator CorrectedRotation = FQuat(-Rotation.x, Rotation.z, Rotation.y, Rotation.w).Rotator();
	CorrectedRotation.Yaw = -CorrectedRotation.Yaw;
	const FVector CorrectedPosition = FVector(Position.x * 100, Position.z * 100, Position.y * 100);

	FTransform Transform(CorrectedRotation, CorrectedPosition, FVector(Scale.x, Scale.z, Scale.y));

	if (Transform.GetScale3D().IsZero())
	{
		Transform.SetScale3D(FVector(1));
	}
	return Transform;
}

FString UAssimpFunctionLibrary::GetBoneName(FAIBone Bone)
{
	return UTF8_TO_TCHAR(Bone.Bone->mName.C_Str());
}

int UAssimpFunctionLibrary::GetNumOfWeights(FAIBone Bone)
{
	return Bone.Bone->mNumWeights;
}

FTransform UAssimpFunctionLibrary::GetBoneTransform(FAIBone Bone)
{
	return UAssimpFunctionLibrary::aiMatToTransform(Bone.Bone->mOffsetMatrix);
}

void UAssimpFunctionLibrary::GetBoneWeights(FAIBone Bone, TArray<FAIVertexWeight> &Weights)
{
	for (unsigned int i = 0; i < Bone.Bone->mNumWeights; i++)
	{
		FAIVertexWeight Weight = FAIVertexWeight(Bone.Bone->mWeights[i]);
		Weights.Add(Weight);
	}
}
