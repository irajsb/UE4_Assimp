// Fill out your copyright notice in the Description page of Project Settings.

#include "AIBone.h"
#include "AssimpFunctionLibrary.h"
#include "assimp/mesh.h"

FAIBone::FAIBone()
{
    Bone = nullptr;
}

FAIBone::FAIBone(aiBone *Bone)
{
    this->Bone = Bone;
}

FAIVertexWeight::FAIVertexWeight()
{
}

FAIVertexWeight::FAIVertexWeight(aiVertexWeight InVertexWeight)
{
    VertexID = InVertexWeight.mVertexId;
    Weight = InVertexWeight.mWeight;
}
