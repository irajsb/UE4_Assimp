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
//wrapper for scene
UCLASS(BlueprintType,DefaultToInstanced)
class UE_ASSIMP_API UAIScene : public UObject
{
	GENERATED_BODY()


	public:
	void SetScene(const aiScene* in);
	UFUNCTION(BlueprintCallable)
	TArray<UMeshComponent*>SpawnAllMeshes(FTransform Transform,TSubclassOf<AActor>ClassToSpawn);
	UFUNCTION(BlueprintCallable,BlueprintPure)
	void  GetAllMeshes(TArray<UAIMesh*>& Meshes);
	private:
	const aiScene* scene;
	virtual void BeginDestroy() override;
	
};
