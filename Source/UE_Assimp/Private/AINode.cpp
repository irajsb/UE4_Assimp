// Fill out your copyright notice in the Description page of Project Settings.


#include "AINode.h"

#include "AssimpFunctionLibrary.h"
#include "ThirdParty/UE_AssimpLibrary/assimp/contrib/poly2tri/poly2tri/sweep/advancing_front.h"




const TArray<UAINode*>& UAINode::GetChildNodes() const
{
		return  OwnedNodes;
}

FString UAINode::GetNodeName() const
{

return  UTF8_TO_TCHAR(	Node->mName.C_Str());

}

UAINode* UAINode::GetParentNode(bool& Success) const
{
	Success=false;

UAINode* Parent= Cast<UAINode>(GetOuter());
if(Parent)
	Success=true;
return Parent;	
}

void UAINode::GetNodeTransform(FTransform& Transform) const
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

void UAINode::RegisterNewNode(aiNode* InNode)
{
	
	UAINode* Object=	NewObject<UAINode>(this,UAINode::StaticClass(),NAME_None,RF_Transient);
	Object->Node= InNode;
	OwnedNodes.Add(Object);

	if(InNode->mNumChildren!=0)
	{
		//Follow Chain of nodes to init child nodes 
		for(unsigned Index=0;Index<InNode->mNumChildren;Index++)
		{
			Object->RegisterNewNode(InNode->mChildren[Index]);
		}
		
	}
	
}


