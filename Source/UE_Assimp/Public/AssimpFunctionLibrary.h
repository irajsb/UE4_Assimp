// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AIScene.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AssimpFunctionLibrary.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnProgressUpdated,float,NormalPercentage,UAIScene* ,ImportedScene);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnImportSceneComplete,const TArray<UAIScene*>&,ImportedScenes,float ,TotalTimeElapsed);
UCLASS()
class UE_ASSIMP_API UAssimpFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	public:

	/** 
Opens the "open file" dialog for the platform
 DialogTitle				The text for the title of the dialog window
DefaultPath				The path where the file dialog will open initially
 DefaultFile				The file that the dialog will select initially
 Flags						Details about the dialog. See EFileDialogFlags.
FileTypes					The type filters to show in the dialog. This string should be a "|" delimited list of (Description|Extensionlist) pairs. Extensionlists are ";" delimited.
 OutFilenames				The filenames that were selected in the dialog
 Success  true if files were successfully selected	*/
UFUNCTION(BlueprintCallable)
static void OpenFileDialogue(FString DialogTitle,FString DefaultPath,FString DefaultFile, const FString& FileTypes, uint8 Flags, TArray<FString>& OutFilenames,bool MultiSelect,bool &Success);

//from Engine\Source\Developer\DesktopPlatform\Private\Windows
static bool FileDialogShared(bool bSave, const void* ParentWindowHandle, const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags, TArray<FString>& OutFilenames, int32& OutFilterIndex,bool MultiSelect);
//
UFUNCTION(BlueprintCallable)
 static void ImportScenesAsync(TArray<FString> InFilenames,UObject* ParentObject,FOnProgressUpdated OnProgressUpdated,FOnImportSceneComplete OnImportSceneComplete);
 UFUNCTION(BlueprintCallable)
 static void ImportScenes(TArray<FString> InFilenames,UObject* ParentObject,TArray<UAIScene*>& Scenes);
 static FTransform aiMatToTransform(aiMatrix4x4 NodeTransform);
};
