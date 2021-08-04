// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AIBone.h"
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





 //Bones

 //! The name of the bone.
 UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Bone")
 static FString GetBoneName(FAIBone Bone);
 
 //! The number of vertices affected by this bone.
 //! The maximum value for this member is #AI_MAX_BONE_WEIGHTS.
 UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Bone")
 static int GetNumOfWeights(FAIBone Bone);

 //! The influence weights of this bone, by vertex index.
 //TODO C_STRUCT aiVertexWeight *mWeights;



 /**Assimp: Matrix that transforms from bone space to mesh space in bind pose.
 *
 * This matrix describes the position of the mesh
 * in the local space of this bone when the skeleton was bound.
 * Thus it can be used directly to determine a desired vertex position,
 * given the world-space transform of the bone when animated,
 * and the position of the vertex in mesh space.
 *
 * It is sometimes called an inverse-bind matrix,
 * or inverse bind pose matrix.
 * UE: Matrix Converted to Transform
 */
 UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Bone")
 static FTransform GetBoneTransform(FAIBone Bone);

	
 // The bone armature node - used for skeleton conversion
 // you must enable aiProcess_PopulateArmatureData to populate this
 //	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Bone")
 //	UAINode* GetArmatureNode();

 // The bone node in the scene - used for skeleton conversion
 // you must enable aiProcess_PopulateArmatureData to populate this
 //	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Bone")
 //	UAINode* GetNode();

 //! The influence weights of this bone, by vertex index.
 UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Bone")
 static void GetBoneWeights(FAIBone Bone,TArray<FAIVertexWeight>& Weights);


 
};
