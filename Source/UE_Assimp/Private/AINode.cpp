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
	WorldTransform = UAssimpFunctionLibrary::aiMatToTransform(MyTransform);

	for (unsigned Index = 0; Index < Node->mNumChildren; Index++)
	{
	        UAINode* KidNode = NewObject<UAINode>(this, UAINode::StaticClass(), NAME_None, RF_Transient);
	        KidNode->Setup(Node->mChildren[Index], Scene, MyTransform);
	        OwnedNodes.Add(KidNode);
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

