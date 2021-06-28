// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "assimp/scene.h"
#include "UObject/NoExportTypes.h"
#include "AIScene.generated.h"

/**
 * 
 */

//wrapper for scene
UCLASS(BlueprintType,DefaultToInstanced)
class UE_ASSIMP_API UAIScene : public UObject
{
	GENERATED_BODY()


	public:
	void SetScene(const aiScene* in);
	UFUNCTION(BlueprintCallable)
	TArray<UMeshComponent*>SpawnMeshes(FTransform Transform,TSubclassOf<AActor>ClassToSpawn);
	private:
	const aiScene* scene;
	virtual void BeginDestroy() override;
	
};
