// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBone.h"
#include "assimp/mesh.h"
#include "AssimpFunctionLibrary.h"

FAIBone::FAIBone()
{
	Bone=nullptr;
}

FAIBone::FAIBone(aiBone* Bone)
{
	this->Bone=Bone;
}

FAIVertexWeight::FAIVertexWeight()
{
	
}

FAIVertexWeight::FAIVertexWeight(aiVertexWeight InVertexWeight)
{
	VertexID=InVertexWeight.mVertexId;
	Weight=InVertexWeight.mWeight;
	
}
