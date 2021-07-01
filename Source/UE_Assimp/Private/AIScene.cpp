// Fill out your copyright notice in the Description page of Project Settings.


#include "AIScene.h"

#include "AICamera.h"
#include "AIMesh.h"
#include "AINode.h"
#include "assimp/mesh.h"
#include "AssimpMesh.h"
#include "ProceduralMeshComponent.h"
#include "assimp/cimport.h"


UAIScene* UAIScene::InternalConstructNewScene(UObject* Parent, const aiScene* Scene)
{

	//todo check if object is already created and skip creation and return object 
	UAIScene* Object=	NewObject<UAIScene>(Parent,UAIScene::StaticClass(),NAME_None,RF_Transient);
	Object->scene= const_cast<aiScene*>(Scene);
	
	return Object;
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

	aiReleaseImport( scene);
	scene=nullptr;
	
	Super::BeginDestroy();
}


UAINode* UAIScene::GetRootNode()
{
	return	UAINode::InternalCreateNewObject(this,scene->mRootNode); 
}

UAIMesh* UAIScene::GetMeshAtIndex(int Index)
{
return  UAIMesh::InternalConstructNewAIMesh(scene->mMeshes[Index],this);	
}

void UAIScene::GetAllCameras(bool &HasCameras,TArray<UAICamera*>& Cameras)
{
	Cameras.Reset();

	HasCameras=scene->HasCameras();
	for (unsigned i = 0; i < scene->mNumMeshes; i++)
	{
	Cameras.Add(	UAICamera::InternalCreateNewObject(this,scene->mCameras[i]));	
	}
	
}


