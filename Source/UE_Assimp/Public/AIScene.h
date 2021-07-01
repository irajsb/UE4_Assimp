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
//wrapper for scene
UCLASS(BlueprintType,DefaultToInstanced)
class UE_ASSIMP_API UAIScene : public UObject
{
	GENERATED_BODY()


	public:
	
	static UAIScene* InternalConstructNewScene(UObject* Parent , const aiScene* Scene);
	UFUNCTION(BlueprintCallable,Category="Assimp|Scene")
	TArray<UMeshComponent*>SpawnAllMeshes(FTransform Transform,TSubclassOf<AActor>ClassToSpawn);
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Scene")
	void  GetAllMeshes(TArray<UAIMesh*>& Meshes);
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Scene")
	UAINode* GetRootNode();
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Scene")
	UAIMesh* GetMeshAtIndex(int Index);
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="Assimp|Scene")
	void GetAllCameras(bool& HasCameras,TArray<UAICamera*>& Cameras);
	private:
	 aiScene* scene;
	virtual void BeginDestroy() override;
	
};
