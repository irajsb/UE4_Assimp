// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "assimp/scene.h"
#include "UObject/NoExportTypes.h"
#include "AIScene.generated.h"

/**
 * 
 */
class UAIMesh;
class UAINode;
class UAICamera;
class UAILight;
class UAIMaterial;
//wrapper for scene
UCLASS(BlueprintType,DefaultToInstanced)
class UE_ASSIMP_API UAIScene : public UObject
{
	GENERATED_BODY()

//TODO Get Meta data 
	public:
	
	static UAIScene* InternalConstructNewScene(UObject* Parent , const aiScene* Scene);

	/*WIP Function:
	WIll spawn all meshes in most optimised fashion 
	*/
	UFUNCTION(BlueprintCallable,Category="Assimp|Scene")
	TArray<UMeshComponent*>SpawnAllMeshes(FTransform Transform,TSubclassOf<AActor>ClassToSpawn);
	/*Get All meshes stored in this scene
	* note that each material section is considered a  separate mesh 
	*/
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Scene")
	 const TArray<UAIMesh*>&  GetAllMeshes() const;
	/** The root node of the hierarchy.
	*
	* There will always be at least the root node if the import
	* was successful (and no special flags have been set).
	* Presence of further nodes depends on the format and content
	* of the imported file.
	*/
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Scene")
	UAINode* GetRootNode();
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Scene")
	UAIMesh* GetMeshAtIndex(int Index);
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Scene")
	const TArray<UAICamera*>& GetAllCameras() const;





	



	void RegisterNewNode(aiNode* InRootNode);

	private:

	
	

	//For Object Creation
	UPROPERTY(Transient)
	TArray<UAIMesh*> OwnedMeshes;
	UPROPERTY(Transient)
	UAINode*	OwnedRootNode;
	UPROPERTY(Transient)
	TArray<UAICamera*>OwnedCameras;
	UPROPERTY(Transient)
	TArray<UAILight*>OwnedLights;
	UPROPERTY(Transient)
	TArray<UAIMaterial*>OwnedMaterials;
	 aiScene* scene;

	
	virtual void BeginDestroy() override;
	
};
