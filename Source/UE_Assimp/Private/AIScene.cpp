// Fill out your copyright notice in the Description page of Project Settings.


#include "AIScene.h"

#include "AIMesh.h"
#include "assimp/mesh.h"
#include "AssimpMesh.h"
#include "ProceduralMeshComponent.h"
#include "ThirdParty/UE_AssimpLibrary/assimp/code/AssetLib/3MF/3MFXmlTags.h"

void UAIScene::SetScene(const aiScene* in)
{
	scene=in;
}

TArray<UMeshComponent*> UAIScene::SpawnAllMeshes(FTransform Transform,TSubclassOf<AActor>ClassToSpawn)
{
	TArray<UMeshComponent*> Meshes;
	if(scene)
	{

		for (int i = 0; i < (int)scene->mNumMeshes; i++)
		{
			auto mesh = scene->mMeshes[i];
			//mesh data will be owned by the scene object
			UAIMesh* NewMeshData=UAIMesh::InternalConstructNewAIMesh(mesh,this);
			AActor* SpawnedActor=	GetWorld()->SpawnActor<AActor>(ClassToSpawn,Transform);
			UAssimpMesh* AssimpMesh=	NewObject<UAssimpMesh>(SpawnedActor,UAssimpMesh::StaticClass());
			AssimpMesh->RegisterComponent();
			AssimpMesh->SetWorldLocation(Transform.GetLocation()); 
			AssimpMesh->SetWorldRotation(Transform.GetRotation()); 
			AssimpMesh->AttachToComponent(SpawnedActor->GetRootComponent(),FAttachmentTransformRules::KeepWorldTransform);
			AssimpMesh->SetupMesh(NewMeshData);
			
		}
		
	}else
	{
		UE_LOG(LogTemp,Error,TEXT(" Assimp scene is not valid "));
		 
	}
	

return Meshes;
}

void UAIScene::GetAllMeshes(TArray<UAIMesh*>& Meshes)
{

	for (int i = 0; i < (int)scene->mNumMeshes; i++)
	{
		auto mesh = scene->mMeshes[i];
		//mesh data will be owned by the scene object
		
		
		if(!mesh)
		{
			UE_LOG(LogTemp,Error,TEXT(" Imported mesh not valid at index %d"),i);
		}else
		{
			UAIMesh* NewMeshData=	UAIMesh::InternalConstructNewAIMesh(mesh,this);
			Meshes.Add(NewMeshData);
		}
		
	}
	
}

void UAIScene::BeginDestroy()
{

	scene=nullptr;
	
	Super::BeginDestroy();
}
