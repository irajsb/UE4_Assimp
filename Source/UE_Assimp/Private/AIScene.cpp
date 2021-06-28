// Fill out your copyright notice in the Description page of Project Settings.


#include "AIScene.h"
#include "assimp/mesh.h"
#include "ProceduralMeshComponent.h"
void UAIScene::SetScene(const aiScene* in)
{
	scene=in;
}

TArray<UMeshComponent*> UAIScene::SpawnMeshes(FTransform Transform,TSubclassOf<AActor>ClassToSpawn)
{
	TArray<UMeshComponent*> Meshes;
	if(scene)
	{

		for (int i = 0; i < (int)scene->mNumMeshes; i++)
		{
			auto mesh = scene->mMeshes[i];

			
		}
		
	}else
	{
		UE_LOG(LogTemp,Error,TEXT(" Assimp scene is not valid "));
		 
	}
	

return Meshes;
}

void UAIScene::BeginDestroy()
{

	scene=nullptr;
	
	Super::BeginDestroy();
}
