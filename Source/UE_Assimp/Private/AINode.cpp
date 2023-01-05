// Fill out your copyright notice in the Description page of Project Settings.


#include "AINode.h"

#include "AssimpFunctionLibrary.h"
#include "AIMesh.h"


const TArray<UAINode*>& UAINode::GetChildNodes() const
{
	return OwnedNodes;
}

void UAINode::Setup(aiNode* InNode, UAIScene* Scene, const aiMatrix4x4& ParentTransform)
{
	this->Node = InNode;

	

	const aiMatrix4x4t<float> MyTransform = ParentTransform * Node->mTransformation;

	for (unsigned Index = 0; Index < Node->mNumMeshes; Index++)
	{
	
		UAIMesh* NewMeshData = NewObject<UAIMesh>(Scene, UAIMesh::StaticClass(), NAME_None, RF_Transient);
		NewMeshData->Mesh = Scene->scene->mMeshes[Node->mMeshes[Index]];
		WorldTransform = UAssimpFunctionLibrary::aiMatToTransform(MyTransform);
		if (Scene->SceneScale != 0)
		{
			WorldTransform.SetScale3D(WorldTransform.GetScale3D() * Scene->SceneScale);
		}
	
		Scene->OwnedMeshes[Node->mMeshes[Index]] = NewMeshData;
		
	}

	for (unsigned Index = 0; Index < Node->mNumChildren; Index++)
	{
		RegisterNewNode(Node->mChildren[Index], Scene, MyTransform);
	}
}

FString UAINode::GetNodeName() const
{
	return UTF8_TO_TCHAR(Node->mName.C_Str());
}

UAINode* UAINode::GetParentNode(bool& Success) const
{
	Success = false;

	UAINode* Parent = Cast<UAINode>(GetOuter());
	if (Parent)
		Success = true;
	return Parent;
}

void UAINode::GetNodeTransform(FTransform& Transform) const
{
	Transform = UAssimpFunctionLibrary::aiMatToTransform(Node->mTransformation);
}

FTransform UAINode::GetRootTransform()
{
	return WorldTransform;
}

bool UAINode::GetNodeMeshes(TArray<UAIMesh*>& Meshes)
{
	UAIScene* Scene = GetScene();
	if (Scene)
	{
		Meshes.Reset();
		if (Node->mNumMeshes == 0)
			return false;
		for (unsigned Index = 0; Index < Node->mNumMeshes; Index++)
		{
			Meshes.Add(Scene->GetMeshAtIndex(Node->mMeshes[Index]));
		}
	}
	else
	{
		UE_LOG(LogAssimp, Error, TEXT("No Scene Found "));
		return false;
	}
	return true;
}

UAIScene* UAINode::GetScene()
{
	UObject* Outer = GetOuter();
	while (Outer)
	{
		UAIScene* Scene = Cast<UAIScene>(Outer);
		if (Scene)
		{
			return Scene;
		}
		else
		{
			Outer = Outer->GetOuter();
		}
	}
	return nullptr;
}

void UAINode::RegisterNewNode(aiNode* InNode, UAIScene* Scene, const aiMatrix4x4& ParentTransform)
{
	UAINode* Object = NewObject<UAINode>(this, UAINode::StaticClass(), NAME_None, RF_Transient);
	Object->Setup(InNode, Scene, ParentTransform);
	OwnedNodes.Add(Object);
}
