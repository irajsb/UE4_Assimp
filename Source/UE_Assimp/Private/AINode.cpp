// Fill out your copyright notice in the Description page of Project Settings.


#include "AINode.h"

#include "AssimpFunctionLibrary.h"
#include "ThirdParty/UE_AssimpLibrary/assimp/contrib/poly2tri/poly2tri/sweep/advancing_front.h"


UAINode* UAINode::InternalCreateNewObject(UObject* Parent, aiNode* InNode)
{

	//todo check if object is already created and skip creation and return object 
	UAINode* Object=	NewObject<UAINode>(Parent,UAINode::StaticClass(),NAME_None,RF_Transient);
	Object->Node= InNode;
	return Object;
}

void UAINode::GetChildNodes(bool& Empty,TArray<UAINode*>& Nodes)
{
	
	Nodes.Reset();
	if(Node->mNumChildren==0)
	{
		Empty=true;
		return;
	}else
	{
		for (unsigned Index=0 ;Index<Node->mNumChildren;Index++)
		{
			UAINode* NewNodeObject=UAINode::InternalCreateNewObject(this,Node->mChildren[Index]);
			Nodes.Add(NewNodeObject);
		}
		Empty=false;
	}
	
}

FString UAINode::GetNodeName()
{

return  UTF8_TO_TCHAR(	Node->mName.C_Str());

}

UAINode* UAINode::GetParentNode(bool& Success)
{
	Success=false;

UAINode* Parent= Cast<UAINode>(GetOuter());
if(Parent)
	Success=true;
return Parent;	
}

void UAINode::GetNodeTransform(FTransform& Transform)
{
Transform= UAssimpFunctionLibrary::aiMatToTransform(Node->mTransformation);
}

bool UAINode::GetAllMeshes(TArray<UAIMesh*>& Meshes)
{
	
	UAIScene* Scene=	GetScene();
	if(Scene)
	{
		Meshes.Reset();
		if(Node->mNumMeshes==0)
			return false;
		for (unsigned Index=0 ;Index<Node->mNumMeshes;Index++)
		{
		Meshes.Add(Scene->GetMeshAtIndex(Node->mMeshes[Index]));
		}
	}else
	{
		UE_LOG(LogTemp,Error,TEXT("No Scene Found "));
		return  false;
	}
	return true;
	
}

UAIScene* UAINode::GetScene()
{
	UObject* Outer=GetOuter();
	while (Outer)
	{
		UAIScene* Scene=	Cast<UAIScene>(Outer);
		if(Scene)
		{
			return Scene;
		}else
		{
			Outer=Outer->GetOuter();
			
		}
	}
	return nullptr;
}


